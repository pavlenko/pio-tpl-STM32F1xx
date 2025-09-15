Import("env")

config = env.GetProjectConfig()

board_config = env.BoardConfig()
board_config.update("build.hwids", [
  [config.get("extra", "USB_APP_VID"), config.get("extra", "USB_APP_PID")],
  [config.get("extra", "USB_DFU_VID"), config.get("extra", "USB_DFU_PID")],
])

if "boot" == env["PIOENV"]:
  board_config.update("upload.maximum_size", int(config.get("extra", "F103C8xx_BOOT_SIZE"), 0))

if "user" == env["PIOENV"]:
  board_config.update("upload.offset_address", '%#X' % (int("0x08000000", 0) + int(config.get("extra", "F103C8xx_BOOT_SIZE"), 0)))
  board_config.update("upload.maximum_size", board_config.get("upload.maximum_size") - int(config.get("extra", "F103C8xx_BOOT_SIZE"), 0))

cxx = env["CXXFLAGS"]
cxx = [x for x in cxx if not "-std" in x]
cxx += ["-std=c++2b"]
env.Replace(CXXFLAGS=cxx)