[Setup]
AppId={{E7239C3A-3A3B-4A3A-B72A-9F00670E7294}
AppName=Makosizer
AppVersion=1.0.0
AppPublisher=MAKOSIZER
DefaultDirName={autopf}\Common Files\VST3
DisableProgramGroupPage=yes
OutputDir=..\..\dist
OutputBaseFilename=Makosizer_Setup
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "..\..\build\Makosizer_artefacts\Release\VST3\Makosizer.vst3\*"; DestDir: "{commoncf}\VST3\Makosizer.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion

[Code]
function InitializeSetup(): Boolean;
begin
  Result := IsWin64;

  if not Result then
    MsgBox(
      'Makosizer VST3 requires 64-bit Windows.',
      mbError,
      MB_OK
    );
end;
