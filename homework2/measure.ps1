# 在專案根目錄執行，參數填入已編譯好的主程式路徑。
param([string]$Program = '.\homework2.exe')
$ErrorActionPreference = 'Stop'
foreach ($n in @(10, 100, 1000)) {
    $parts = @("$n")
    for ($i = 0; $i -lt $n; ++$i) { $parts += "1 $i" }
    $inputText = ($parts -join ' ') + ' ' + ($parts -join ' ') + ' 1'
    $times = @()
    for ($repeat = 0; $repeat -lt 5; ++$repeat) {
        $watch = [System.Diagnostics.Stopwatch]::StartNew()
        $inputText | & $Program | Out-Null
        if ($LASTEXITCODE -ne 0) { throw 'Program failed' }
        $watch.Stop()
        $times += $watch.Elapsed.TotalMilliseconds
    }
    $average = ($times | Measure-Object -Average).Average
    Write-Output ('n={0}, average={1:F3} ms (5 runs)' -f $n, $average)
}
