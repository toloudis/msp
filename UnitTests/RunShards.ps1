<#
    RunShards.ps1

    Runs a GoogleTest exe as several processes at once, each running a
    share of the tests (gtest sharding: GTEST_TOTAL_SHARDS and
    GTEST_SHARD_INDEX), then prints each shard's output in turn and merges
    their XML reports into one. Used to run GraphicsTests by hand (see
    GraphicsTests/README.md): its tests spend most of their time waiting on device and layer start-up,
    so they run several at a time. -Shards defaults to 8 (fewer on a
    machine with fewer cores): more gave no gain on a 12-core machine.

    The exit code is 0 only if every shard passed. Extra arguments go to
    every shard (e.g. --gtest_filter=..., --gtest_brief=1).

    Example:
      powershell -NoProfile -ExecutionPolicy Bypass -File RunShards.ps1 `
          -Exe GraphicsTests.exe -Xml GraphicsTests.xml --gtest_brief=1
#>
param(
    [Parameter(Mandatory = $true)][string]$Exe,
    [Parameter(Mandatory = $true)][string]$Xml,
    [int]$Shards = [Math]::Min(8, [Environment]::ProcessorCount),
    [Parameter(ValueFromRemainingArguments = $true)][string[]]$GtestArgs
)

$ErrorActionPreference = 'Stop'

$Exe = (Resolve-Path $Exe).Path
$Xml = [System.IO.Path]::GetFullPath($Xml)
$work = Join-Path ([System.IO.Path]::GetTempPath()) ("RunShards-" + [System.IO.Path]::GetFileNameWithoutExtension($Exe) + "-" + $PID)
New-Item -ItemType Directory -Force $work | Out-Null

$clock = [System.Diagnostics.Stopwatch]::StartNew()
$runs = @()
try {
    for ($i = 0; $i -lt $Shards; $i++) {
        $xmlPart = Join-Path $work "shard$i.xml"
        $arguments = @("--gtest_output=xml:`"$xmlPart`"")
        if ($GtestArgs) { $arguments += $GtestArgs }
        # Start-Process passes on the current environment
        $env:GTEST_TOTAL_SHARDS = "$Shards"
        $env:GTEST_SHARD_INDEX = "$i"
        $process = Start-Process -FilePath $Exe -ArgumentList $arguments -NoNewWindow -PassThru `
            -WorkingDirectory (Split-Path $Exe) `
            -RedirectStandardOutput (Join-Path $work "shard$i.out") -RedirectStandardError (Join-Path $work "shard$i.err")
        $null = $process.Handle    # keeps the exit code readable after the process ends
        $runs += [pscustomobject]@{ Index = $i; Process = $process; Xml = $xmlPart }
    }
}
finally {
    Remove-Item Env:GTEST_TOTAL_SHARDS, Env:GTEST_SHARD_INDEX -ErrorAction SilentlyContinue
}

$failed = @()
foreach ($run in $runs) {
    $run.Process.WaitForExit()
    $code = $run.Process.ExitCode
    Write-Output "===== shard $($run.Index + 1) of $Shards (exit code $code) ====="
    foreach ($stream in "out", "err") {
        $file = Join-Path $work "shard$($run.Index).$stream"
        if (Test-Path $file) { Get-Content $file | Write-Output }
    }
    if ($code -ne 0 -or -not (Test-Path $run.Xml)) { $failed += $run.Index + 1 }
}
$seconds = $clock.Elapsed.TotalSeconds

# Merge the reports: test suites split across shards become one again
$merged = New-Object System.Xml.XmlDocument
$root = $merged.AppendChild($merged.CreateElement("testsuites"))
$counts = [ordered]@{ tests = 0; failures = 0; disabled = 0; errors = 0 }
$suites = [ordered]@{}
foreach ($run in $runs) {
    if (-not (Test-Path $run.Xml)) { continue }
    $part = New-Object System.Xml.XmlDocument
    $part.Load($run.Xml)
    foreach ($name in @($counts.Keys)) { $counts[$name] += [int]$part.DocumentElement.GetAttribute($name) }
    if (-not $root.HasAttribute("timestamp")) { $root.SetAttribute("timestamp", $part.DocumentElement.GetAttribute("timestamp")) }
    foreach ($suite in $part.DocumentElement.SelectNodes("testsuite")) {
        $name = $suite.GetAttribute("name")
        if (-not $suites.Contains($name)) {
            $suites[$name] = $root.AppendChild($merged.ImportNode($suite, $false))
        }
        else {
            $into = $suites[$name]
            foreach ($attribute in "tests", "failures", "disabled", "skipped", "errors") {
                $into.SetAttribute($attribute, [string]([int]$into.GetAttribute($attribute) + [int]$suite.GetAttribute($attribute)))
            }
            $into.SetAttribute("time", ([double]$into.GetAttribute("time") + [double]$suite.GetAttribute("time")).ToString("0.###", [System.Globalization.CultureInfo]::InvariantCulture))
        }
        foreach ($case in $suite.ChildNodes) { $null = $suites[$name].AppendChild($merged.ImportNode($case, $true)) }
    }
}
foreach ($name in $counts.Keys) { $root.SetAttribute($name, [string]$counts[$name]) }
$root.SetAttribute("time", $seconds.ToString("0.###", [System.Globalization.CultureInfo]::InvariantCulture))
$root.SetAttribute("name", "AllTests")
$merged.Save($Xml)

Remove-Item -Recurse -Force $work -ErrorAction SilentlyContinue

$summary = "{0} tests in {1} shards, {2:0.0} s: {3} failed" -f $counts.tests, $Shards, $seconds, $counts.failures
if ($failed.Count -gt 0) {
    Write-Output "$summary; shard(s) $($failed -join ', ') failed or crashed (output above)"
    exit 1
}
Write-Output $summary
exit 0
