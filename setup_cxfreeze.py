import sys
from cx_Freeze import setup, Executable

build_exe_options = {"packages": ["os", "io", "random", "getpass",
	"subprocess", "urllib2", "htmlentitydefs", "HTMLParser",
	"xml.parsers.expat", "ctypes", "gzip", "json", "pipes",
	"platform", "xml.etree.ElementTree", "ctypes.wintypes",
	"netrc", "uuid", "shutil"],
	"include_files": ["youtube_dl/"]}

base = None
#if sys.platform == "win32":
#	base = "Win32GUI"

setup(name="youtube_dl",
      options={"build_exe": build_exe_options},
      executables=[Executable("youtube_dl/__main__.py", base=base)])
