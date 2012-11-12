@echo off
:: ***********************************************************************************
::  File:
::	do_swigall.bat
::
::  Description:
::	swig の実行が必要かどうかを判定/実行させるための Makefile を作成するための
::	スクリプト. 
::	関連するファイル：
::	  input:  swigprojs.list(=%LISTFILE%)
::			swig 実行の対象とするプロジェクトを記述したファイル.
::			.../Springhead2/test/bat(=%ETCDIR%) に置かれているものとする.
::	  output: Makefile.swig(=%MAKEFILENAME%)
::			swig を実行させるための makefile.
::			各プロジェクトのビルドディレクトリに出力される
::			(=.../Springhead2/src/[project]/Makefile.swig)
:: ***********************************************************************************
::  Version:
::	Ver 1.0	2012/10/25	F.Kanehori
:: ***********************************************************************************
setlocal enabledelayedexpansion
set CWD=%cd%

:: ----------
::  各種定義
:: ----------
:: 関連するヘッダファイルがあるディレクトリ
::
:: (make を実行するディレクトリからみた相対パス)
set INCDIR=../../include
set SRCDIR=../../src
set ETCDIR=../../src/swig/etc
::
:: (build を実行するディレクトリからみた相対パス)
set ACTINCDIR=../../include
set ACTSRCDIR=.

:: 使用するファイル名
::
set LISTFILE=swigprojs.list
set MAKEFILENAME=Makefile.swig
set STUBFILE=Stub.cpp
set TMPFILE=swigprojs.tmp

:: 使用するプログラムとパス
::
set PATH=C:\Program Files\Microsoft Visual Studio 10.0\Common7\IDE;..\..\src\Foundation;%PATH%
set NMAKE=nmake
set SWIG=RunSwig.bat

:: 常に依存関係にあるファイルの一覧
::
set FIXDEPSINC=%ACTINCDIR%/Springhead.h %ACTINCDIR%/Base/Env.h %ACTINCDIR%/Base/BaseDebug.h
set FIXDEPSSRC=%ACTSRCDIR%/../Foundation/UTTypeDesc.h
set FIXHDRS=%FIXDEPSINC% %FIXDEPSSRC%

:: 依存関係にはないと見做すファイルの一覧
::
set EXCLUDES=

:: ----------
::  処理開始
:: ----------
:: echo start at %CWD%
for /f "tokens=1,*" %%p in (%ETCDIR%\%LISTFILE%) do (
    echo.
    echo *** %%p ***
    echo.

    rem 依存関係にあるファイルの一覧を、変数 *HDRS に集める.
    rem     毎回ディレクトリのリストをとるので、新規のファイルが追加されても大丈夫.
    rem	    サブルーチン parse は、依存関係にあるプロジェクトについて処理を行なう.
    rem	 ※ Makefile マクロのデリミタと batch subroutine 引数のデリミタが同じなので
    rem	    面倒である (!*HDRS! を1つの引数に纏められれば簡単になる). 
    rem	    サブルーチン :add_headers の結果は環境変数 STR1 に、:add_prefix の結果は
    rem	    環境変数 STR2 に設定される.
    rem
    rem	依存関係を定義したファイル swigprojs.list (=%LISTFILE%) の書式は次のとおり.
    rem	    <line> ::= <project> ' ' <dependency-list>
    rem	    <project> ::= 処理の対象とするプロジェクト名 (=モジュール名)
    rem	    <dependency-list> ::= ε| <dependent-project> ',' <dependency-list>
    rem	    <dependent-project> ::= 依存関係にあるプロジェクト名 (=モジュール名)
    rem ※ 少々見難いが、<project>と<dependency-list>の間区切り文字は＊空白1個＊、
    rem	   <dependency-list>の要素間の区切り文字は＊カンマ1個＊に限る.
    rem
    rem echo FIXHDRS [%FIXHDRS%]

    set INCHDRS=
    for %%d in (%INCDIR%/%%p/*.h) do (
	call :add_headers !INCHDRS! %%d
	set INCHDRS=!STR1!
    )
    call :add_prefix %ACTINCDIR%/%%p
    set INCHDRS=!STR2!
    if "%%q" neq "" (
	call :parse %INCDIR% "%%q" %TMPFILE% "init"
	for /f %%e in (%TMPFILE%) do (
	    set INCHDRS=!INCHDRS! %%e
	)
	del %TMPFILE% 2> nul
    )
    rem echo INCHDRS [!INCHDRS!]

    set SRCHDRS=
    for %%d in (%SRCDIR%/%%p/*.h) do (
	call :add_headers !SRCHDRS! %%d
	set SRCHDRS=!STR1!
    )
    call :add_prefix %ACTSRCDIR%
    set SRCHDRS=!STR2!
    if "%%q" neq "" (
	call :parse %SRCDIR% "%%q" %TMPFILE% "init"
	for /f %%e in (%TMPFILE%) do (
	    set SRCHDRS=!SRCHDRS! %%e
	)
	del %TMPFILE% 2> nul
    )
    rem echo SRCHDRS [!SRCHDRS!]

    rem Makefile を作成する.
    rem
    set MAKEFILE=%SRCDIR%/%%p/%MAKEFILENAME%
    echo making !MAKEFILE!
    echo # THIS FILE IS AUTO-GENERATED. ** DO NOT EDIT THIS FILE ** > !MAKEFILE!
    echo # File: %MAKEFILENAME%	>> !MAKEFILE!
    echo.  			>> !MAKEFILE!
    echo FIXHDRS=\>> !MAKEFILE!
    for %%h in (!FIXHDRS!) do (
	echo %%h\>> !MAKEFILE!
    )
    echo.  			>> !MAKEFILE!
    echo INCHDRS=\>> !MAKEFILE!
    for %%h in (!INCHDRS!) do (
	echo %%h\>> !MAKEFILE!
    )
    echo.  			>> !MAKEFILE!
    echo SRCHDRS=\>> !MAKEFILE!
    for %%h in (!SRCHDRS!) do (
	echo %%h\>> !MAKEFILE!
    )
    echo.  			>> !MAKEFILE!
    echo all:	%%p%STUBFILE%	>> !MAKEFILE!
    echo.  			>> !MAKEFILE!
    echo %%p%STUBFILE%:	$^(FIXHDRS^) $^(INCHDRS^) $^(SRCHDRS^)	>> !MAKEFILE!

    set SWIGARGS=%%p
    if "%%q" neq "" (
	set SWIGOPTS=%%q
	set SWIGARGS=!SWIGARGS! !SWIGOPTS:,= !
    )
    echo.	%SWIG% !SWIGARGS!	>> !MAKEFILE!

    echo.  			>> !MAKEFILE!
    echo $^(FIXHDRS^):		>> !MAKEFILE!
    echo.  			>> !MAKEFILE!
    echo $^(INCHDRS^):		>> !MAKEFILE!
    echo.  			>> !MAKEFILE!
    echo $^(SRCHDRS^):		>> !MAKEFILE!

    rem 上で作成した Makefile を実行する.
    rem     依存関係に列挙したファイルに変更があったときのみ swig が実行される.
    rem
    cd %SRCDIR%/%%p
    %NMAKE% -f %MAKEFILENAME%
    cd %CWD%
)

:: ----------
::  処理終了
:: ----------
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  変数 *HDRS にファイル名を追加する (デリミタは空白文字).
::
::  引数 %1	変数 *HDRS の現在値
::	 %2	*HDRS に追加するファイル名
:: -----------------------------------------------------------------------------------
:add_headers
set STR1=
:loop
    if "%1" equ "" goto :endloop
    for %%f in (%EXCLUDES%) do if %1 equ %%f goto :next
    if "!STR1!" equ "" (
	set STR1=%1
    ) else (
	set STR1=!STR1! %1
    )
:next
    shift /1
    goto :loop
:endloop
exit /b

:: -----------------------------------------------------------------------------------
::  引数で与えられたカンマ区切りの文字列を解析し、要素(＝依存するプロジェクト名)毎に
::  サブルーチン dirlist を呼び出してヘッダ情報の収集を行なう.
::
::  引数 %1	走査するディレクトリのベースパス
::	 %2	依存するプロジェクトのリスト (カンマ区切りのリスト)
::	 %3	作業用ファイル名
::	 %4	作業用ファイル初期化フラグ (何かが設定されていたら初期化を行なう)
:: -----------------------------------------------------------------------------------
:parse
setlocal
set LIST=%2
set LIST=%LIST:"=%
for /f "delims=, tokens=1,*" %%a in ("%LIST%") do (
    if "%4" neq "" del %3 2> nul
    if "%%a" neq "" call :dirlist %1 %%a %3
    if "%%b" neq "" call :parse %1 "%%b" %3
)
endlocal
exit /b

:: -----------------------------------------------------------------------------------
::  引数で与えられたプロジェクト<p>について、include/<p> ディレクトリに存在するヘッダ
::  ファイルをリストアップして作業ファイルに書き出す (1ファイル/1行). 
::
::  引数 %1	走査するディレクトリのベースパス
::	 %2	プロジェクト名
::	 %3	作業用ファイル名
:: -----------------------------------------------------------------------------------
:dirlist
set DIR=%2
set DIR=%DIR:"=%
echo dipendent module: [%1/%DIR%]
for %%d in (%1/%DIR%/*.h) do (
    rem echo output: [%1/%DIR%/%%d]
    echo %1/%DIR%/%%d >> %3
)
exit /b

:: -----------------------------------------------------------------------------------
::  変数 STR1 に設定されている各ファイル名に、引数で指定された prefix を追加する.
::  結果は、環境変数 STR2 に設定される.
::
::  引数 %1	追加するプリフィックス
:: -----------------------------------------------------------------------------------
:add_prefix
set STR2=
for %%f in (!STR1!) do (
    if "!STR2!" equ "" (
	set STR2=%1/%%f
    ) else (
	set STR2=!STR2! %1/%%f
    )
)
exit /b

