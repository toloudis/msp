#!/usr/bin/env python3
"""
fx2hlsl.py - one-time conversion of DX11 Effects (.fx) files to plain HLSL.

For each input Foo.fx this writes, next to it:

  Foo.hlsl         the shader source with every Effects-only construct removed:
                   technique11/pass blocks, SamplerState initializers and
                   <...> annotation blocks. A generated section at the end adds
                   one plain entry point per pass stage that passed uniform
                   arguments in its compile statement, so every shader in the
                   effect can be compiled with an ordinary vs_/ps_/hs_/ds_/gs_
                   target (fxc or dxc).
  Foo.effect.json  everything the Effects runtime used to get from the .fx:
                   techniques -> passes -> per-stage entry point and profile,
                   sampler state descriptions, and per-variable semantics,
                   annotations (the SAS material-UI metadata) and default
                   values (plain HLSL drops global initializers).

Local #include files are converted the same way to .hlsli (no entry points) and
the #include lines are rewritten to point at them.

Nothing in the shader math is changed. The original source is rewritten only
by deleting spans, so comments, macros and layout survive and the output can
be hand-edited afterwards. Structure (techniques, function signatures,
variables) is read from a preprocessed copy so macro-generated passes are seen.

Usage:
  python fx2hlsl.py [--root SHADER_ROOT] [--include-dir DIR ...] FILE.fx [...]
  python fx2hlsl.py --check FILE.fx    # parse and report only, write nothing

Requires: Python 3.8+, pcpp (pip install pcpp).
"""

import argparse
import io
import json
import os
import re
import sys

try:
    import pcpp
except ImportError:
    sys.exit("fx2hlsl needs pcpp: python -m pip install pcpp")

STAGES = {
    "Vertex": "vs", "Pixel": "ps", "Hull": "hs",
    "Domain": "ds", "Geometry": "gs", "Compute": "cs",
}

# An annotation block starts like "< string Name =" (or is empty: "< >").
ANNOTATION_START = re.compile(
    r"<\s*(?:>|(?:string|bool|int|uint|half|float|double)\d?(?:x\d)?\s+\w+\s*=)")

SAMPLER_TYPES = ("SamplerState", "SamplerComparisonState")

# Parameter types that are always varying (never bound from a compile statement).
VARYING_TYPE = re.compile(
    r"^(InputPatch|OutputPatch|PointStream|LineStream|TriangleStream)\b")
VARYING_MODIFIERS = {"point", "line", "triangle", "lineadj", "triangleadj"}
PARAM_MODIFIERS = {
    "in", "out", "inout", "uniform", "const", "precise", "nointerpolation",
    "linear", "centroid", "noperspective", "sample",
} | VARYING_MODIFIERS


class ConvertError(Exception):
    pass


# ---------------------------------------------------------------------------
# Lexical helpers
# ---------------------------------------------------------------------------

def mask(text):
    """Return a copy of text, same length, with comments, string literals and
    preprocessor directive lines replaced by spaces (newlines kept), so that
    structure can be found with regexes and brace matching. Positions in the
    masked text map 1:1 to the original."""
    out = list(text)
    n = len(text)

    def blank(a, b):
        for k in range(a, b):
            if out[k] != "\n":
                out[k] = " "

    i = 0
    line_start = True
    while i < n:
        c = text[i]
        if text.startswith("//", i):
            j = text.find("\n", i)
            j = n if j < 0 else j
            blank(i, j)
            i = j
            continue
        if text.startswith("/*", i):
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            blank(i, j)
            i = j
            continue
        if c == '"':
            j = i + 1
            while j < n and text[j] != '"' and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            j = min(j + 1, n)
            blank(i, j)
            i = j
            line_start = False
            continue
        if c == "#" and line_start:
            j = i
            while j < n:
                if text[j] == "\n" and text[j - 1] != "\\" and not text[j - 2:j] == "\\\r":
                    break
                j += 1
            blank(i, j)
            i = j
            continue
        if c == "\n":
            line_start = True
        elif not c.isspace():
            line_start = False
        i += 1
    return "".join(out)


CLOSERS = {"(": ")", "[": "]", "{": "}"}


def match_close(m, i):
    """m[i] is an opening bracket; return the index of its closing partner."""
    stack = []
    for k in range(i, len(m)):
        c = m[k]
        if c in CLOSERS:
            stack.append(CLOSERS[c])
        elif c in ")]}":
            if not stack or stack.pop() != c:
                raise ConvertError("unbalanced '%s' at offset %d" % (c, k))
            if not stack:
                return k
    raise ConvertError("unclosed '%s' at offset %d" % (m[i], i))


def annotation_end(m, i):
    """m[i] is '<' starting an annotation block; return the index of its '>'."""
    depth = 0
    for k in range(i + 1, len(m)):
        c = m[k]
        if c in "({[":
            depth += 1
        elif c in ")}]":
            depth -= 1
        elif c == ">" and depth == 0:
            return k
    raise ConvertError("unclosed annotation at offset %d" % i)


def split_top(s, sep=",", angles=False):
    """Split s on sep at bracket depth 0 (optionally also treating <> as
    brackets, for template types like InputPatch<T, 3> in parameter lists)."""
    parts, depth, cur = [], 0, []
    opens, closes = ("({[<", ")}]>") if angles else ("({[", ")}]")
    for c in s:
        if c in opens:
            depth += 1
        elif c in closes:
            depth -= 1
        if c == sep and depth == 0:
            parts.append("".join(cur))
            cur = []
        else:
            cur.append(c)
    parts.append("".join(cur))
    return [p.strip() for p in parts if p.strip()]


def ws(s):
    return " ".join(s.split())


def line_of(text, pos):
    return text.count("\n", 0, pos) + 1


# ---------------------------------------------------------------------------
# Values (annotations, sampler states, variable defaults)
# ---------------------------------------------------------------------------

def parse_value(raw):
    """Best-effort conversion of an HLSL initializer to JSON. Anything that is
    not a literal (or a constructor/brace list of literals) is kept as text."""
    v = raw.strip()
    if len(v) >= 2 and v[0] == '"' and v[-1] == '"':
        return v[1:-1]
    if v in ("true", "false"):
        return v == "true"
    ctor = re.match(r"^(?:float|int|uint|half|bool|double)\d?(?:x\d)?\s*\((.*)\)$", v, re.S)
    if ctor:
        v = "{" + ctor.group(1) + "}"
    if v.startswith("{") and v.endswith("}"):
        return [parse_value(x) for x in split_top(v[1:-1])]
    num = re.match(r"^[-+]?(\d+\.?\d*|\.\d+)([eE][-+]?\d+)?[fFhHlLuU]?$", v)
    if num:
        core = v.rstrip("fFhHlLuU")
        return float(core) if re.search(r"[.eE]", core) else int(core)
    if re.match(r"^\w+$", v):
        return v                          # enum-like identifier, e.g. CLAMP
    return {"expr": ws(v)}


def parse_assignments(text):
    """Parse 'type? name = value;' lists (annotation and sampler bodies)."""
    result = {}
    for stmt in split_top(text, ";"):
        if "=" not in stmt:
            continue
        lhs, rhs = stmt.split("=", 1)
        lhs = lhs.split()
        name = lhs[-1]
        entry = parse_value(rhs)
        if len(lhs) > 1:
            entry = {"type": lhs[0], "value": entry}
        result[name] = entry
    return result


# ---------------------------------------------------------------------------
# Structure in the preprocessed text
# ---------------------------------------------------------------------------

class Source:
    """Preprocessed text plus its mask."""

    def __init__(self, text):
        self.text = text
        self.m = mask(text)


class _Preprocessor(pcpp.Preprocessor):
    def include(self, tokens, original_line):
        # the shaders use Windows-style '..\\Support.h' include paths
        for t in tokens:
            if t.type == self.t_STRING:
                t.value = t.value.replace("\\", "/")
        return super().include(tokens, original_line)


def preprocess(path, include_dirs):
    pp = _Preprocessor()
    pp.line_directive = None
    pp.compress = 0
    for d in include_dirs:
        pp.add_path(d)
    errors = []
    pp.on_error = lambda file, line, msg: errors.append("%s:%s: %s" % (file, line, msg))
    with open(path, encoding="utf-8", errors="replace") as f:
        pp.parse(f.read(), path)
    out = io.StringIO()
    pp.write(out)
    if errors:
        raise ConvertError("preprocessor errors:\n  " + "\n  ".join(errors))
    return out.getvalue()


def struct_semantics(src):
    """Names of structs whose members carry semantics (these are varying)."""
    names = set()
    for mt in re.finditer(r"\bstruct\s+(\w+)\s*\{", src.m):
        body_end = match_close(src.m, mt.end() - 1)
        body = src.m[mt.end():body_end]
        if re.search(r":\s*\w+\s*;", body):
            names.add(mt.group(1))
    return names


class Function:
    def __init__(self, name, attrs, ret, params, ret_semantic):
        self.name = name
        self.attrs = attrs              # raw attribute text, e.g. [maxvertexcount(3)]
        self.ret = ret                  # return type
        self.params = params            # list of dicts
        self.ret_semantic = ret_semantic

    def uniform_params(self):
        return [p for p in self.params if p["uniform"]]


def parse_param(raw, varying_structs):
    text = raw
    default = None
    if "=" in text:
        text, default = text.split("=", 1)
        default = default.strip()
    semantic = None
    sm = re.search(r":\s*(\w+)\s*$", text)
    if sm:
        semantic = sm.group(1)
        text = text[:sm.start()]
    text = text.strip()
    am = re.search(r"(\[[^\]]*\]\s*)+$", text)
    if am:
        text = text[:am.start()].strip()
    toks = re.findall(r"[\w:]+(?:\s*<[^>]*>)?", text)
    if not toks:
        raise ConvertError("can't parse parameter '%s'" % raw)
    name = toks[-1]
    mods = [t for t in toks[:-1] if t in PARAM_MODIFIERS]
    typ = " ".join(t for t in toks[:-1] if t not in PARAM_MODIFIERS)
    if "uniform" in mods:
        uniform = True
    elif semantic or set(mods) & (VARYING_MODIFIERS | {"out", "inout"}):
        uniform = False
    elif VARYING_TYPE.match(typ) or typ in varying_structs:
        uniform = False
    else:
        uniform = True
    return {"raw": raw.strip(), "name": name, "type": typ, "semantic": semantic,
            "default": default, "uniform": uniform}


def find_functions(src, name, varying_structs):
    """All definitions of function `name` in the preprocessed source."""
    found = []
    m = src.m
    for mt in re.finditer(r"\b%s\s*\(" % re.escape(name), m):
        open_paren = mt.end() - 1
        close = match_close(m, open_paren)
        after = re.match(r"\s*(?::\s*(\w+)\s*)?\{", m[close + 1:])
        if not after:
            continue                      # a call, not a definition
        before = m[:mt.start()].rstrip()
        rt = re.search(r"([\w:]+(?:\s*<[^>]*>)?)$", before)
        if not rt or rt.group(1) in ("return", "else"):
            continue
        ret = ws(rt.group(1))
        k = rt.start()
        attrs = []
        while True:
            pre = m[:k].rstrip()
            if not pre.endswith("]"):
                break
            depth, j = 0, len(pre) - 1
            while j >= 0:
                if pre[j] == "]":
                    depth += 1
                elif pre[j] == "[":
                    depth -= 1
                    if depth == 0:
                        break
                j -= 1
            attrs.insert(0, ws(src.text[j:len(pre)]))
            k = j
        params = [parse_param(p, varying_structs)
                  for p in split_top(src.m[open_paren + 1:close], angles=True)
                  if ws(p) != "void"]
        found.append(Function(name, attrs, ret, params, after.group(1)))
    return found


PASS_STMT = [
    # VertexShader = compile vs_5_0 Func(args)
    re.compile(r"^(\w+)Shader\s*=\s*compile\s+(\w+)\s+(\w+)\s*\((.*)\)$", re.S),
    # SetVertexShader(CompileShader(vs_5_0, Func(args)))
    re.compile(r"^Set(\w+)Shader\s*\(\s*CompileShader\s*\(\s*(\w+)\s*,\s*(\w+)\s*\((.*)\)\s*\)\s*\)$", re.S),
]
PASS_NULL = [
    re.compile(r"^(\w+)Shader\s*=\s*NULL$"),
    re.compile(r"^Set(\w+)Shader\s*\(\s*NULL\s*\)$"),
]


def parse_annotation_block(src, i):
    """src.m[i] == '<'. Returns (annotations dict, index after '>')."""
    end = annotation_end(src.m, i)
    return parse_assignments(src.text[i + 1:end]), end + 1


def parse_techniques(src):
    techniques = []
    m = src.m
    for mt in re.finditer(r"\btechnique1[01]?\s+(\w+)\s*", m):
        k = mt.end()
        annotations = {}
        if m[k] == "<":
            annotations, k = parse_annotation_block(src, k)
            k = len(m[:k]) + len(m[k:]) - len(m[k:].lstrip())
        if m[k] != "{":
            raise ConvertError("technique %s: expected '{'" % mt.group(1))
        tech_end = match_close(m, k)
        tech = {"name": mt.group(1), "passes": []}
        if annotations:
            tech["annotations"] = annotations
        for mp in re.finditer(r"\bpass\s+(\w+)\s*", m[k:tech_end]):
            p = k + mp.end()
            pass_annotations = {}
            if m[p] == "<":
                pass_annotations, p = parse_annotation_block(src, p)
                p += len(m[p:]) - len(m[p:].lstrip())
            if m[p] != "{":
                raise ConvertError("pass %s: expected '{'" % mp.group(1))
            pass_end = match_close(m, p)
            entry = {"name": mp.group(1), "stages": {}}
            if pass_annotations:
                entry["annotations"] = pass_annotations
            for stmt in split_top(m[p + 1:pass_end], ";"):
                stmt = ws(stmt)
                parsed = None
                for rx in PASS_STMT:
                    sm = rx.match(stmt)
                    if sm:
                        stage, profile, func, args = sm.groups()
                        parsed = (stage, {"profile": profile, "function": func,
                                          "args": split_top(args)})
                        break
                if not parsed:
                    for rx in PASS_NULL:
                        sm = rx.match(stmt)
                        if sm:
                            parsed = (sm.group(1), None)
                            break
                if not parsed or parsed[0] not in STAGES:
                    raise ConvertError("technique %s pass %s: unsupported statement '%s'"
                                       % (tech["name"], entry["name"], stmt))
                entry["stages"][STAGES[parsed[0]]] = parsed[1]
            tech["passes"].append(entry)
        techniques.append(tech)
    return techniques


def top_level_statements(src):
    """Yield (start, end) spans of depth-0 statements in the masked text, with
    annotation blocks skipped over. A statement ends at ';' or at a '}' that
    closes a function/technique body."""
    m = src.m
    n = len(m)
    i = 0
    start = 0
    while i < n:
        c = m[i]
        if c == "<" and ANNOTATION_START.match(m, i):
            i = annotation_end(m, i) + 1
            continue
        if c in "({[":
            close = match_close(m, i)
            if c == "{":
                nxt = m[close + 1:].lstrip()
                if not nxt.startswith(";") and not nxt.startswith(","):
                    yield start, close + 1
                    i = start = close + 1
                    continue
            i = close + 1
            continue
        if c == ";":
            yield start, i + 1
            start = i + 1
        i += 1


GLOBAL_SKIP = re.compile(
    r"^(static|const|struct|cbuffer|tbuffer|typedef|technique1[01]?|namespace|class|interface)\b")


def parse_globals(src):
    """Return (variables, samplers) from the preprocessed source."""
    variables, samplers = {}, {}
    for a, b in top_level_statements(src):
        stmt_m = src.m[a:b].strip()
        if not stmt_m or GLOBAL_SKIP.match(stmt_m):
            continue
        # strip annotation blocks, but keep their content
        annotations = {}
        text_parts, m_parts = [], []
        k = a
        while k < b:
            if src.m[k] == "<" and ANNOTATION_START.match(src.m, k):
                ann, nk = parse_annotation_block(src, k)
                annotations.update(ann)
                k = nk
                continue
            text_parts.append(src.text[k])
            m_parts.append(src.m[k])
            k += 1
        stmt_m = "".join(m_parts).strip()
        stmt_t = "".join(text_parts).strip()
        # function definitions: '(' before the first '{' and no '=' before '('
        brace = stmt_m.find("{")
        paren = stmt_m.find("(")
        eq = stmt_m.find("=")
        if brace >= 0 and 0 <= paren < brace and (eq < 0 or eq > paren):
            continue
        if stmt_m.startswith(SAMPLER_TYPES):
            sm = re.match(r"^(\w+)\s+(\w+)\s*(\[[^\]]*\])?\s*(?::\s*\w+\s*)?(\{.*\})?\s*;?$",
                          stmt_m, re.S)
            if not sm:
                raise ConvertError("can't parse sampler '%s'" % ws(stmt_t))
            desc = {}
            if sm.group(4):
                body_start = stmt_m.index("{")
                desc = parse_assignments(stmt_t[body_start + 1:stmt_t.rindex("}")])
            samplers[sm.group(2)] = {"type": sm.group(1), "state": desc}
            if sm.group(3):
                samplers[sm.group(2)]["array"] = sm.group(3)
            continue
        dm = re.match(r"^(.*?)\s*(?:=\s*(.*?))?\s*;$", stmt_t, re.S)
        if not dm:
            continue
        decl, default = dm.group(1), dm.group(2)
        sem = None
        sm = re.search(r":\s*(\w+)\s*$", decl)
        if sm:
            sem = sm.group(1)
            decl = decl[:sm.start()]
        vm = re.match(r"^(.*?)\b(\w+)\s*((?:\[[^\]]*\]\s*)*)$", decl.strip(), re.S)
        if not vm:
            continue
        typ = ws(vm.group(1))
        typ = re.sub(r"\b(uniform|extern|shared)\s+", "", typ)
        var = {"type": typ}
        if vm.group(3).strip():
            var["array"] = ws(vm.group(3))
        if sem:
            var["semantic"] = sem
        if annotations:
            var["annotations"] = annotations
        if default is not None:
            var["default"] = parse_value(default)
        variables[vm.group(2)] = var
    return variables, samplers


# ---------------------------------------------------------------------------
# Rewriting the original source
# ---------------------------------------------------------------------------

def removal_spans(text):
    """Spans of the original text to delete: technique blocks, sampler state
    initializers and annotation blocks."""
    m = mask(text)
    spans = []
    for mt in re.finditer(r"\btechnique1[01]?\s+\w+\s*", m):
        k = mt.end()
        if m[k] == "<":
            k = annotation_end(m, k) + 1
            k += len(m[k:]) - len(m[k:].lstrip())
        spans.append((mt.start(), match_close(m, k) + 1))
    for mt in re.finditer(r"\b(?:%s)\s+\w+\s*(?:\[[^\]]*\])?\s*(?::\s*\w+\s*)?\{"
                          % "|".join(SAMPLER_TYPES), m):
        brace = mt.end() - 1
        # remove from the end of the declarator (incl. whitespace) to the '}'
        decl_end = len(m[:brace].rstrip())
        spans.append((decl_end, match_close(m, brace) + 1))
    for mt in re.finditer(r"<", m):
        if ANNOTATION_START.match(m, mt.start()):
            if any(a <= mt.start() < b for a, b in spans):
                continue
            # also swallow the whitespace before '<'
            start = len(m[:mt.start()].rstrip())
            spans.append((start, annotation_end(m, mt.start()) + 1))
    spans.sort()
    merged = []
    for a, b in spans:
        if merged and a < merged[-1][1]:
            merged[-1] = (merged[-1][0], max(b, merged[-1][1]))
        else:
            merged.append((a, b))
    return merged


def rewrite(text, include_map):
    out, last = [], 0
    for a, b in removal_spans(text):
        out.append(text[last:a])
        last = b
    out.append(text[last:])
    result = "".join(out)

    def fix_include(mt):
        name = mt.group(2)
        return mt.group(1) + include_map.get(name, name) + mt.group(3)
    result = re.sub(r'(^\s*#\s*include\s*")([^"]+)(")', fix_include, result, flags=re.M)
    # collapse the blank runs left behind by deleted technique blocks
    return re.sub(r"\n{4,}", "\n\n\n", result)


# ---------------------------------------------------------------------------
# Entry points
# ---------------------------------------------------------------------------

def build_entry_points(src, techniques):
    """Resolve every pass stage to an entry point; return wrapper source.
    Stages without compile arguments use the function directly."""
    varying_structs = struct_semantics(src)
    wrappers = []
    by_key = {}
    for tech in techniques:
        for p in tech["passes"]:
            for stage, sh in p["stages"].items():
                if sh is None:
                    continue
                func, args = sh.pop("function"), sh.pop("args")
                key = (stage, func, tuple(ws(a) for a in args))
                if key in by_key:
                    sh["entry"] = by_key[key]
                    continue
                candidates = find_functions(src, func, varying_structs)
                if not candidates:
                    raise ConvertError("%s/%s: function %s not found"
                                       % (tech["name"], p["name"], func))
                fn = None
                for c in candidates:
                    uni = c.uniform_params()
                    required = [u for u in uni if u["default"] is None]
                    if len(required) <= len(args) <= len(uni):
                        fn = c
                        break
                if fn is None:
                    raise ConvertError("%s/%s: %s() takes %s uniform parameter(s), compile passes %d"
                                       % (tech["name"], p["name"], func,
                                          "/".join(str(len(c.uniform_params())) for c in candidates),
                                          len(args)))
                if not args:
                    sh["entry"] = func
                else:
                    entry = "%s_%s_%s" % (tech["name"], p["name"], stage.upper())
                    wrappers.append(make_wrapper(fn, entry, args))
                    sh["entry"] = entry
                by_key[key] = sh["entry"]
    return "\n".join(wrappers)


def make_wrapper(fn, entry, args):
    varying = [p for p in fn.params if not p["uniform"]]
    call, ai = [], 0
    for p in fn.params:
        if p["uniform"]:
            if ai < len(args):
                call.append(ws(args[ai]))
            ai += 1
        else:
            call.append(p["name"])
    while call and ai > len(args) and fn.params[len(call) - 1]["uniform"]:
        call.pop()     # trailing uniforms left to their defaults
        ai -= 1
    lines = list(fn.attrs)
    sig = "%s %s(%s)" % (fn.ret, entry, ", ".join(ws(p["raw"]) for p in varying))
    if fn.ret_semantic:
        sig += " : " + fn.ret_semantic
    lines.append(sig)
    lines.append("{")
    body = "%s(%s);" % (fn.name, ", ".join(call))
    lines.append("    " + ("" if fn.ret == "void" else "return ") + body)
    lines.append("}")
    return "\n".join(lines) + "\n"


# ---------------------------------------------------------------------------
# Driver
# ---------------------------------------------------------------------------

def dump_json(obj):
    """json.dumps with lists of scalars kept on one line."""
    text = json.dumps(obj, indent=2)
    return re.sub(r"\[\s*([^\[\]{}]*?)\s*\]",
                  lambda mt: "[" + ", ".join(x.strip() for x in mt.group(1).split(",")) + "]"
                  if mt.group(1).strip() else "[]", text)


def rel(path, root):
    return os.path.relpath(path, root).replace(os.sep, "/")


HEADER = """\
//////////////////////////////////////////////////////////////////////////////
// Converted from {src} by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
// Techniques, sampler states, annotations and variable defaults now live in
// {manifest}. This file is the source of truth from here on.
//////////////////////////////////////////////////////////////////////////////

"""

INCLUDE_HEADER = """\
//////////////////////////////////////////////////////////////////////////////
// Converted from {src} by Tools/fx2hlsl/fx2hlsl.py (one-time conversion).
//////////////////////////////////////////////////////////////////////////////

"""

ENTRY_HEADER = """
//////////////////////////////////////////////////////////////////////////////
// Entry points generated from the technique/pass compile statements.
// Each one binds the uniform arguments the .fx passed in its compile call.
//////////////////////////////////////////////////////////////////////////////

"""


def local_includes(path, include_dirs):
    """Local files #included by path (recursively)."""
    found = []
    seen = set()

    def walk(p):
        with open(p, encoding="utf-8", errors="replace") as f:
            text = f.read()
        for mt in re.finditer(r'^\s*#\s*include\s*"([^"]+)"', text, re.M):
            for d in [os.path.dirname(p)] + include_dirs:
                cand = os.path.normpath(os.path.join(d, mt.group(1).replace("\\", "/")))
                if os.path.isfile(cand):
                    if cand not in seen:
                        seen.add(cand)
                        walk(cand)
                        found.append((mt.group(1), cand))
                    break
    walk(path)
    return found


def include_target(name):
    base, ext = os.path.splitext(name.replace("\\", "/"))
    return base + ".hlsli" if ext.lower() in (".h", ".fxh") else name


def convert(path, root, include_dirs, check, log):
    dirs = [os.path.dirname(path)] + include_dirs
    src = Source(preprocess(path, dirs))
    techniques = parse_techniques(src)
    variables, samplers = parse_globals(src)
    wrappers = build_entry_points(src, techniques)

    base = os.path.splitext(path)[0]
    hlsl_path = base + ".hlsl"
    manifest_path = base + ".effect.json"
    includes = local_includes(path, include_dirs)
    include_map = {name: include_target(name) for name, _ in includes}

    manifest = {
        "source": rel(path, root),
        "hlsl": os.path.basename(hlsl_path),
        "techniques": techniques,
        "samplers": samplers,
        "variables": variables,
    }

    n_passes = sum(len(t["passes"]) for t in techniques)
    n_wrappers = wrappers.count("\n{\n")
    log("%s: %d technique(s), %d pass(es), %d generated entry point(s), "
        "%d sampler(s), %d variable(s), %d annotated"
        % (rel(path, root), len(techniques), n_passes, n_wrappers, len(samplers),
           len(variables), sum(1 for v in variables.values() if "annotations" in v)))
    if check:
        return

    with open(path, encoding="utf-8", errors="replace") as f:
        original = f.read()
    with open(hlsl_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(HEADER.format(src=os.path.basename(path),
                              manifest=os.path.basename(manifest_path)))
        f.write(rewrite(original, include_map).rstrip() + "\n")
        if wrappers:
            f.write(ENTRY_HEADER)
            f.write(wrappers)
    with open(manifest_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(dump_json(manifest) + "\n")

    for name, inc_path in includes:
        target = os.path.join(os.path.dirname(inc_path), os.path.basename(include_target(name)))
        if target == inc_path or os.path.exists(target) and os.path.getmtime(target) > os.path.getmtime(inc_path):
            continue
        with open(inc_path, encoding="utf-8", errors="replace") as f:
            inc_text = f.read()
        with open(target, "w", encoding="utf-8", newline="\n") as f:
            f.write(INCLUDE_HEADER.format(src=os.path.basename(inc_path)))
            f.write(rewrite(inc_text, include_map).rstrip() + "\n")
        log("  wrote %s" % rel(target, root))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("files", nargs="+", help=".fx files to convert")
    ap.add_argument("--root", default=None,
                    help="shader root, for relative paths in output (default: common dir)")
    ap.add_argument("--include-dir", action="append", default=[],
                    help="extra #include search directory (repeatable)")
    ap.add_argument("--check", action="store_true",
                    help="parse and report only; write nothing")
    args = ap.parse_args()

    files = [os.path.abspath(f) for f in args.files]
    root = os.path.abspath(args.root) if args.root else os.path.commonpath(
        [os.path.dirname(f) for f in files])
    include_dirs = [os.path.abspath(d) for d in args.include_dir] + [root]
    failed = 0
    for f in files:
        try:
            convert(f, root, include_dirs, args.check, print)
        except ConvertError as e:
            failed += 1
            print("%s: FAILED: %s" % (rel(f, root), e), file=sys.stderr)
    if failed:
        print("%d of %d file(s) failed" % (failed, len(files)), file=sys.stderr)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
