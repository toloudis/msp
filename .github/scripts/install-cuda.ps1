<#
    install-cuda.ps1

    Installs the parts of the CUDA toolkit MachStudio's build needs (nvcc,
    to compile GraphicsOptiX's device programs to OptiX IR, and the CUDA
    headers) into -Destination, from NVIDIA's redistributable archives:
    no installer, no administrator rights. The archives are listed in
    https://developer.download.nvidia.com/compute/cuda/redist/redistrib_<version>.json.

    The result is laid out as a toolkit install (bin\nvcc.exe, include\cuda.h,
    nvvm\...), so LibXLT/GraphicsOptiX/cuda.props finds it through CUDA_PATH.

    Used by .github/workflows/build-windows.yml. Written for PowerShell 7
    and Windows PowerShell 5.1.
#>
param(
    [Parameter(Mandatory = $true)][string]$Version,
    [Parameter(Mandatory = $true)][string]$Destination
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

# nvcc and its front end, the C runtime headers nvcc compiles device code
# with (cuda_crt, split out of cuda_nvcc in CUDA 13), the CUDA headers
# (cuda.h, for the driver API types), CCCL (libcu++ headers such as
# nv/target, which the CUDA headers include) and NVVM, which generates
# OptiX IR
$Components = @("cuda_nvcc", "cuda_crt", "cuda_cudart", "cccl", "libnvvm")
$Platform = "windows-x86_64"
$Base = "https://developer.download.nvidia.com/compute/cuda/redist"

$manifest = Invoke-RestMethod "$Base/redistrib_$Version.json"
$work = Join-Path ([IO.Path]::GetTempPath()) "cuda-redist-$([Guid]::NewGuid())"
New-Item -ItemType Directory -Force -Path $work, $Destination | Out-Null
try {
    foreach ($name in $Components) {
        $entry = $manifest.$name.$Platform
        if (-not $entry) { throw "CUDA $Version has no $name for $Platform" }
        $zip = Join-Path $work "$name.zip"
        Write-Host "Downloading $name $($manifest.$name.version)"
        Invoke-WebRequest "$Base/$($entry.relative_path)" -OutFile $zip
        $sha = (Get-FileHash $zip -Algorithm SHA256).Hash
        if ($sha -ne $entry.sha256.ToUpper()) { throw "$name failed its SHA-256 check" }

        # each archive holds one top-level folder (<name>-<platform>-<version>-archive)
        # with the toolkit's layout under it; merge them all into -Destination
        $unpacked = Join-Path $work $name
        Expand-Archive $zip -DestinationPath $unpacked
        $root = Get-ChildItem $unpacked -Directory | Select-Object -First 1
        Get-ChildItem $root.FullName -Force | Copy-Item -Destination $Destination -Recurse -Force
    }
}
finally {
    Remove-Item $work -Recurse -Force -ErrorAction SilentlyContinue
}

foreach ($f in "bin\nvcc.exe", "include\cuda.h", "include\crt\host_defines.h", "include\cccl\nv\target", "nvvm\bin") {
    if (-not (Test-Path (Join-Path $Destination $f))) { throw "CUDA install in $Destination has no $f" }
}
Write-Host "CUDA $Version (nvcc, headers) installed in $Destination"
