#include "Win.h"

#include <io.h>
#include <fcntl.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <Windows.h>

static const char kDefaultConfig[] = R"(# hacks: [ "savestate", "test" ]

enabled: [ 1 ]

controller:
# Button mapping
- from: A
  to: A
- from: X
  to: B

- from: DpadLeft
  to: DpadLeft
- from: DpadRight
  to: DpadRight
- from: DpadDown
  to: DpadDown
- from: DpadUp
  to: DpadUp
- from: Start
  to: Start

- from: R
  to: R
- from: L
  to: Z

#- from:
#    type: axis
#    axis: 120
#    comparer: More
#    offset: LeftTrigger
#  to: Z
- from: Back
  to: L
#- from:
#    type: axis
#    axis: 120
#    comparer: More
#    offset: RightTrigger
#  to: R

# CButtons Mapping
- from:
    type: axis
    axis: 16000
    offset: RightX
    comparer: More
  to: CRight
- from:
    type: axis
    axis: -16000
    offset: RightX
    comparer: Less
  to: CLeft
- from:
    type: axis
    axis: 16000
    offset: RightY
    comparer: More
  to: CUp
- from:
    type: axis
    axis: -16000
    offset: RightY
    comparer: Less
  to: CDown

# Stick Mapping
- type: bilinear
  fromX:
    center: 0
    max: 32000
    offset: LeftX
  toX:
    center: 0
    max: 90
    offset: X
  fromY:
    center: 0
    max: 32000
    offset: LeftY
  toY:
    center: 0
    max: 90
    offset: Y
  deadzone: 0.02
  angleDeadzone:
    directions: 4
    deadzone: 0.1

# The Framewalk button :)
# Make sure to have this mapping place AFTER stick so it would override the normal stick input
- from: Y
  to:
    type: axis
    axis: 80
    offset: Y

# Keyboard controls
- from: KeyUp
  to:
    type: axis
    axis: 80
    offset: Y
- from: KeyDown
  to:
    type: axis
    axis: -80
    offset: Y
- from: KeyLeft
  to:
    type: axis
    axis: -80
    offset: X
- from: KeyRight
  to:
    type: axis
    axis: 80
    offset: X

- from: KeyZ
  to: Z
- from: KeyX
  to: A
- from: KeyC
  to: B

- from: KeyQ
  to: L  
- from: KeyE
  to: R

- from: KeyW
  to: CUp
- from: KeyS
  to: CDown
- from: KeyA
  to: CLeft
- from: KeyD
  to: CRight

- from: KeyI
  to: DpadUp
- from: KeyK
  to: DpadDown
- from: KeyJ
  to: DpadLeft
- from: KeyL
  to: DpadRight

- from: KeyReturn
  to: Start
)";

namespace Win
{
    std::string ConfigPath()
    {
		static char _strPath[4096] = {};
		if ('\0' != *_strPath)
			return _strPath;

		SHGetFolderPath(NULL,
			CSIDL_APPDATA,
			NULL,
			0,
			_strPath);

		PathAppend(_strPath, "LMapper");
		CreateDirectory(_strPath, nullptr); // can fail, ignore errors
		size_t length = strlen(_strPath);

		PathAppend(_strPath, "cfg.yaml");
		int fd = _open(_strPath, _O_BINARY | _O_WRONLY | _O_CREAT | _O_EXCL, 0666);
		if (-1 != fd)
		{
			_write(fd, kDefaultConfig, sizeof(kDefaultConfig) - 1);
			_close(fd);
		}

        return _strPath;
    }
}
