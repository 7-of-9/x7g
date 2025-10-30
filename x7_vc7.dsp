# Microsoft Developer Studio Project File - Name="x7f_vc6" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=x7f_vc6 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "x7f_vc6.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "x7f_vc6.mak" CFG="x7f_vc6 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "x7f_vc6 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "x7f_vc6 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "x7f_vc6 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ./fmod_api/lib/fmodvc.lib dinput8.lib dsound.lib ddraw.lib dinput.lib d3dxof.lib d3dx8.lib d3dx8d.lib d3dx8dt.lib d3d8.lib winmm.lib dxguid.lib d3dxof.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "x7f_vc6 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FAcs /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ./fmod_api/lib/fmodvc.lib dinput8.lib dsound.lib ddraw.lib dinput.lib d3dxof.lib d3dx8.lib d3dx8d.lib d3dx8dt.lib d3d8.lib winmm.lib dxguid.lib d3dxof.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /profile /debug /machine:I386

!ENDIF 

# Begin Target

# Name "x7f_vc6 - Win32 Release"
# Name "x7f_vc6 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CBillBoard.cpp
# End Source File
# Begin Source File

SOURCE=.\CBoundingBox.cpp
# End Source File
# Begin Source File

SOURCE=.\CCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\CFighter.cpp
# End Source File
# Begin Source File

SOURCE=.\CFileRenderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CHUDScaleBar.cpp
# End Source File
# Begin Source File

SOURCE=.\CHUDText.cpp
# End Source File
# Begin Source File

SOURCE=.\CInGameMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CMainMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\CMenuItem.cpp
# End Source File
# Begin Source File

SOURCE=.\CMenuScrollingBackG.cpp
# End Source File
# Begin Source File

SOURCE=.\CMenuTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\COcean.cpp
# End Source File
# Begin Source File

SOURCE=.\CParticleSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\CProceduralRenderObject.cpp
# End Source File
# Begin Source File

SOURCE=.\CQuadtree.cpp
# End Source File
# Begin Source File

SOURCE=.\CRace.cpp
# End Source File
# Begin Source File

SOURCE=.\CRaceTrack.cpp
# End Source File
# Begin Source File

SOURCE=.\CRaceTrackSegment.cpp
# End Source File
# Begin Source File

SOURCE=.\CScreenOverlay.cpp
# End Source File
# Begin Source File

SOURCE=.\CScreenOverlayManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CSky.cpp
# End Source File
# Begin Source File

SOURCE=.\CSoundManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\CTerrainPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\CTextures.cpp
# End Source File
# Begin Source File

SOURCE=.\CThread.cpp
# End Source File
# Begin Source File

SOURCE=.\CUserFighter.cpp
# End Source File
# Begin Source File

SOURCE=.\CUserInput.cpp
# End Source File
# Begin Source File

SOURCE=.\X7.cpp
# End Source File
# Begin Source File

SOURCE=.\X7_Utils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CBillboard.h
# End Source File
# Begin Source File

SOURCE=.\CBoundingBox.h
# End Source File
# Begin Source File

SOURCE=.\CCamera.h
# End Source File
# Begin Source File

SOURCE=.\CFighters.h
# End Source File
# Begin Source File

SOURCE=.\CFileRenderObject.h
# End Source File
# Begin Source File

SOURCE=.\CHUDScaleBar.h
# End Source File
# Begin Source File

SOURCE=.\CHUDText.h
# End Source File
# Begin Source File

SOURCE=.\CInGameMenu.h
# End Source File
# Begin Source File

SOURCE=.\CMainMenu.h
# End Source File
# Begin Source File

SOURCE=.\CMenuItem.h
# End Source File
# Begin Source File

SOURCE=.\CMenuScrollingBackG.h
# End Source File
# Begin Source File

SOURCE=.\CMenuTextures.h
# End Source File
# Begin Source File

SOURCE=.\COcean.h
# End Source File
# Begin Source File

SOURCE=.\CParticleSystem.h
# End Source File
# Begin Source File

SOURCE=.\CProceduralRenderObject.h
# End Source File
# Begin Source File

SOURCE=.\CQuadtree.h
# End Source File
# Begin Source File

SOURCE=.\CRace.h
# End Source File
# Begin Source File

SOURCE=.\CRaceTrack.h
# End Source File
# Begin Source File

SOURCE=.\CRaceTrackSegment.h
# End Source File
# Begin Source File

SOURCE=.\CScreenOverlay.h
# End Source File
# Begin Source File

SOURCE=.\CScreenOverlayManager.h
# End Source File
# Begin Source File

SOURCE=.\CSky.h
# End Source File
# Begin Source File

SOURCE=.\CSoundManager.h
# End Source File
# Begin Source File

SOURCE=.\CTerrain.h
# End Source File
# Begin Source File

SOURCE=.\CTerrainPatch.h
# End Source File
# Begin Source File

SOURCE=.\CTextures.h
# End Source File
# Begin Source File

SOURCE=.\CThread.h
# End Source File
# Begin Source File

SOURCE=.\CUserInput.h
# End Source File
# Begin Source File

SOURCE=.\tex.h
# End Source File
# Begin Source File

SOURCE=.\X7.h
# End Source File
# Begin Source File

SOURCE=.\x7_primitives.h
# End Source File
# Begin Source File

SOURCE=.\X7_utils.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\directx.Ico
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\winmain.rc
# End Source File
# End Group
# End Target
# End Project
