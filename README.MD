- create single multienv project for f1, f4 mcu
- restructure lib to hardware/software parts
- boards:
  - bluepill (F1)
  - blackpill (F4)
  - G070RB

lib structure:

- inc/stm32/
  - dev/
    - common/_cmsis.hpp - include cmsis heder depends on current mcu
    - common/clock_definitions.hpp - base hw device definitions, without implementations, prevent direct inclusion
    - common/click.hpp - common mcu independent implementations, prevent direct inclusion
    - f1/clock.hpp - mcu dependent implementations, prevent direct inclusion
    - f4/clock.hpp - mcu dependent implementations, prevent direct inclusion
    - clock.hpp - header for include in project, autoresolve depends on mcu
  - lib/
    - atomic/
    - console/
    - delay/
    - dispatcher/
- src/ - source files with subdirs as in inc, may be empty
- library.json