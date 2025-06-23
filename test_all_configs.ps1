# Test all station configurations
# PowerShell script to validate all configuration builds

Write-Host "Testing all FluxTune station configurations..." -ForegroundColor Green

# Store original config file
$configFile = "include\station_config.h"
$backupFile = "include\station_config.h.backup"
Copy-Item $configFile $backupFile

$configs = @(
    "CONFIG_MIXED_STATIONS",
    "CONFIG_FOUR_CW", 
    "CONFIG_FOUR_NUMBERS",
    "CONFIG_FOUR_PAGER",
    "CONFIG_FOUR_RTTY",
    "CONFIG_FOUR_JAMMER",
    "CONFIG_MINIMAL_CW",
    "CONFIG_CW_CLUSTER"
)

$results = @()

foreach ($config in $configs) {
    Write-Host "Testing $config..." -ForegroundColor Yellow
    
    # Create test config
    $content = @"
#ifndef STATION_CONFIG_H
#define STATION_CONFIG_H

// Auto-generated test configuration for $config
#define $config

// Configuration implementation
#ifdef CONFIG_MIXED_STATIONS
    #define ENABLE_MORSE_STATION
    #define ENABLE_NUMBERS_STATION  
    #define ENABLE_PAGER_STATION
    #define ENABLE_RTTY_STATION
#endif

#ifdef CONFIG_FOUR_CW
    #define ENABLE_FOUR_CW_STATIONS
    #define ENABLE_MORSE_STATION
#endif

#ifdef CONFIG_FOUR_NUMBERS
    #define ENABLE_FOUR_NUMBERS_STATIONS
    #define ENABLE_NUMBERS_STATION
#endif

#ifdef CONFIG_FOUR_PAGER
    #define ENABLE_FOUR_PAGER_STATIONS
    #define ENABLE_PAGER_STATION
#endif

#ifdef CONFIG_FOUR_RTTY
    #define ENABLE_FOUR_RTTY_STATIONS
    #define ENABLE_RTTY_STATION
#endif

#ifdef CONFIG_FOUR_JAMMER
    #define ENABLE_FOUR_JAMMER_STATIONS
    #define ENABLE_JAMMER_STATION
#endif

#ifdef CONFIG_MINIMAL_CW
    #define ENABLE_MORSE_STATION
#endif

#ifdef CONFIG_CW_CLUSTER
    #define ENABLE_CW_CLUSTER_STATIONS
    #define ENABLE_MORSE_STATION
#endif

#endif // STATION_CONFIG_H
"@
    
    Set-Content $configFile $content
    
    # Test build
    $buildResult = pio run 2>&1
    if ($LASTEXITCODE -eq 0) {
        $memoryInfo = $buildResult | Select-String "RAM:|Flash:" | ForEach-Object { $_.ToString().Trim() }
        $results += [PSCustomObject]@{
            Config = $config
            Status = "SUCCESS"
            Memory = $memoryInfo -join " | "
        }
        Write-Host "  SUCCESS" -ForegroundColor Green
    } else {
        $results += [PSCustomObject]@{
            Config = $config
            Status = "FAILED"
            Memory = "N/A"
        }
        Write-Host "  FAILED" -ForegroundColor Red
    }
}

# Restore original config
Move-Item $backupFile $configFile -Force

# Display results
Write-Host "`nBuild Test Results:" -ForegroundColor Cyan
$results | Format-Table -AutoSize

Write-Host "All configuration tests completed." -ForegroundColor Green
