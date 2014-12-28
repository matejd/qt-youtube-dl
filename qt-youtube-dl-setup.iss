; Inno Setup

#define MyAppName "qt-youtube-dl"
#define MyAppVersion "2014-12-28"
#define MyAppPublisher "Matej Drame"
#define MyAppURL "https://github.com/matejd/qt-youtube-dl"
#define MyAppExeName "qt-youtube-dl.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{66481498-A3DF-4627-82DA-F744419423A8}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputBaseFilename=qt-youtube-dl
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: ".\build\release\qt-youtube-dl.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\icudt53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\icuin53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\icuuc53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: ".\build\release\youtube_dl_frozen\*"; DestDir: "{app}\youtube_dl_frozen\"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

