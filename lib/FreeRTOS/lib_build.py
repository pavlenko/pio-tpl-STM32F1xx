import sys
import os
import shutil

Import("env")

# For each microcontroller, select the corresponding FreeRTOS port folder:
devices = {
    # libopencm3 macros
    "STM32F0": "ARM_CM0",
    "STM32F1": "ARM_CM3",
    "STM32F2": "ARM_CM3",
    "STM32F3": "ARM_CM4F",
    "STM32F4": "ARM_CM4F",
    "STM32F7": "ARM_CM7/r0p1",
    "STM32L0": "ARM_CM0",
    "STM32L1": "ARM_CM3",
    "STM32L4": "ARM_CM4F",
    "STM32G0": "ARM_CM0",
    "STM32G4": "ARM_CM4F",
    "STM32H7": "ARM_CM4F",
    "GD32F1X0": "ARM_CM3",
    "EFM32TG": "ARM_CM3",
    "EFM32G": "ARM_CM3",
    "EFM32LG": "ARM_CM3",
    "EFM32GG": "ARM_CM3",
    "EFM32HG": "ARM_CM0",
    "EFM32WG": "ARM_CM4F",
    "EZR32WG": "ARM_CM4F",
    "LPC13XX": "ARM_CM3",
    "LPC17XX": "ARM_CM3",
    "LPC43XX_M4": "ARM_CM4F",
    "LPC43XX_M0": "ARM_CM0",
    "SAM3A": "ARM_CM3",
    "SAM3N": "ARM_CM3",
    "SAM3S": "ARM_CM3",
    "SAM3U": "ARM_CM3",
    "SAM3X": "ARM_CM3",
    "SAM4L": "ARM_CM4F",
    "SAMD": "ARM_CM0",
    "VF6XX": "ARM_CM4F",
    "PAC55XX": "ARM_CM4F",
    "LM3S": "ARM_CM3",
    "LM4F": "ARM_CM4F",
    "MSP432E4": "ARM_CM4F",
    "SWM050": "ARM_CM0",

    # Arduino/STM32Cube macros
    "STM32F0xx": "ARM_CM0",
    "STM32F1xx": "ARM_CM3",
    "STM32F2xx": "ARM_CM3",
    "STM32F3xx": "ARM_CM4F",
    "STM32F4xx": "ARM_CM4F",
    "STM32F7xx": "ARM_CM7/r0p1",
    "STM32G0xx": "ARM_CM0",
    "STM32G4xx": "ARM_CM4F",
    "STM32H7xx": "ARM_CM4F",
    "STM32L0xx": "ARM_CM0",
    "STM32L1xx": "ARM_CM3",
    "STM32L4xx": "ARM_CM4F",
    "STM32L5xx": "ARM_CM33/non_secure",
    "STM32WBxx": "ARM_CM4F",
    "STM32WLxx": "ARM_CM4F"
}

port = []
freertos_tag = ""
tag_name = ""

# Check all definitions and try to find matching macro/definition:
for item in env.get("CPPDEFINES", []):
    if isinstance(item, tuple):
        if item[0] in devices:
            port.append(item[0])
        # Look for special tags while parsing the macros
        elif item[0] == 'FREERTOS_TAG':
            freertos_tag = item[1]
    else:
        if item in devices:
            port.append(item)

# Throw exception if no macros match a device name:
if len(port) == 0:
    sys.stderr.write("Error: No device defined for FreeRTOS! Please provide platform manually - options below:\n")
    device_keys = devices.keys()
    s = ' ,'.join(device_keys)
    sys.stderr.write(str(s) + '\n')
    env.Exit(1)
# Throw exception if more than one device name is defined:
elif len(port) > 1:
    sys.stderr.write("Error!: Multiple devices defined: %s\n Using %s as default device.\n" % (" ".join(port), port[0]))
    env.Exit(1)
# Else, add the appropriate port folders to the include path and source filter:
else:
    # TODO change to use serparate FreeRTOS-Kernel & FreeRTOS-CMSIS
    print("FreeRTOS: building for", port[0])
    env.Append(CPPPATH=[os.path.realpath("Source/include")])
    # env.Append(CPPPATH=[os.path.realpath("Source/CMSIS_RTOS")])
    # env.Append(CPPPATH=[os.path.realpath("Source/CMSIS_RTOS_V2")])
    env.Append(CPPPATH=[os.path.realpath(os.path.join("Source/portable/GCC/", devices[port[0]]))])
    env.Replace(SRC_FILTER=[
            "+<Source/>",
            "-<Source/CMSIS_RTOS/>",
            "-<Source/CMSIS_RTOS_V2/>",
            "-<Source/portable/>",
            "+<Source/portable/MemMang/heap_4.c>",
            os.path.join("+<Source/portable/GCC/", devices[port[0]], ">")
        ])
    # FreeRTOSConfig.h is located in the project space, outside of the lib:
    env.Append(CPPPATH=env.get("PROJECT_INCLUDE_DIR", []))