This is a sample project layout for the first few labs.

Please put ALL of your source files in the 'src' directory.


To setup ST-Link Debugger if system resets:

- Select "Flash" / "Configure Flash Tools..." This opens up window "Options for Target 'target'"

- Click "Debug" tab in the newly opened window

- Click "Settings" on left hand side next to "Use: ST-Link Debugger" where "ST-Link Debugger" is the selected option
  in a drop down menu. This opens up window "Cortex-M Target Driver Setup"

- In "Debug" tab of newly opened window, at the "Port:" option, select "SW" from the drop down menu

- In "Trace" tab, select "Trace Enable" and set frequency to 168 MHz

- In "Flash Download" tab, Click "Add" near the bottom of the window, this opens up window "Add Flash Programming Algorithm"

- Select, Description: "STM32F4xx Flash" ; Device Type: "On-chip Flash" ; Device Size "1M"

- Click OK on everything to exit the windows, your ST-Link Debugger should be setup