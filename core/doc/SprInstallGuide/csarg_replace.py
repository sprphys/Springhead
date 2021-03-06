#!/usr/local/bin/python
# -*- coding: utf-8 -*-
# ======================================================================
#  SYNOPSIS:
#	csname_replace [options] func files
#	    func:	'enc': encode csname argument to random string.
#			'dec': decode replaced_argument to original one.
#	    files:	Input file names.
#
#	options:
#	    -c fname:	Intermediate file name to place replaced names.
#	    -s:		Save original file (cf. -t option below).
#	    -t suffix:	Suffix of save file.
#
#  DESCRIPTION:
#	Lwarpmkでセクション毎にhtmlファイルを分割しようとするとき、
#	\chapterなどの引数に漢字が含まれているとエラーとなる。
#	これを防ぐために、次の3つの機能を提供する。
#	  enc:	csの引数をランダムに生成した英数字文字列に変換する。
#	  dec:	encで変換した文字列を元の引数に戻す。
#	  ren:	セクション毎のhtmlファイル名がランダム文字列となって
#		しまうので、これを正しいファイル名に戻す。
#	これらの機能の橋渡しをするため、引数とランダム文字列との対応表
#	を記録する中間ファイルを作成する。
#
#  VERSION:
#	Ver 1.0  2018/11/27 F.Kanehori	First release version.
# ======================================================================
version = '1.0'

import sys
import os
import re
import glob
import random
import subprocess
from optparse import OptionParser

# ----------------------------------------------------------------------
#  Constants
#
prog = sys.argv[0].split(os.sep)[-1].split('.')[0]
#

# ----------------------------------------------------------------------
#  Options
#
usage = 'Usage: %prog [options] func file ...'
parser = OptionParser(usage = usage)
parser.add_option('-c', '--correspondence-file', dest='correspondence_file',
			action='store', default='csarg.replace',
			help='intermediate file name (default: %default)')
parser.add_option('-s', '--save-orginal', dest='save_original',
			action='store_true', default=False,
			help='save original file')
parser.add_option('-t', '--tmp-suffix', dest='suffix',
			action='store', default='csarg.org',
			help='temporary file\'s suffix (default: .%default)')
parser.add_option('-v', '--verbose', dest='verbose',
			action='count', default=0,
			help='set verbose mode')
parser.add_option('-V', '--version', dest='version',
			action='store_true', default=False,
			help='show version')

# ----------------------------------------------------------------------
#  Process for command line
#
(options, args) = parser.parse_args()
if options.version:
	print('%s: Version %s' % (prog, version))
	sys.exit(0)
if len(args) < 2:
	parser.error("incorrect number of arguments")

# get options and input file names
correspondence_file = options.correspondence_file
suffix = options.suffix
verbose = options.verbose
#
func = args[0]
fn_args = args[1:]
fnames = []
for fn in fn_args:
	fnames.extend(glob.glob(fn))

if verbose:
	print('func:     %s' % func)
	print('corres:   %s' % correspondence_file)
	print('suffix:   %s' % suffix)
	print('files:    %s' % fnames)

# ----------------------------------------------------------------------
#  メソッドの定義
# ----------------------------------------------------------------------

#  以下に示すcontrol sequenceの引数を漢字を含まない文字列に変換し
#  変換情報を中間ファイルに記録する
#
def encode():

	#  対象とするコマンドシーケンス
	#
	csnames = ['chapter', 'section', 'subsection', 'subsubsection']
	patt = '|'.join(list(map(lambda x: '\\\\'+x+'\\{([^\\}]*)\\}', csnames)))

	#  置き換える文字列は次の文字から生成する
	#
	seed = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'

	#  TeXのソースを書き換えて対象とするcsの引数から漢字をなくす
	#
	replaces = []
	for f in fnames:
		if verbose:
			print('  encoding %s' % f)
		lines = []
		for line in open(f, 'r', encoding='utf-8'):

			#  対象となるcontrol sequenceが見つかった
			m = re.search(patt, line)
			if m:
				if m.group(1): csarg = m.group(1)
				if m.group(2): csarg = m.group(2)
				if m.group(3): csarg = m.group(3)
				randomname = random_str(seed, 16)
				if verbose:
					print('    %s -> %s' % (csarg, randomname))
				line = line.replace(csarg, randomname)
				replaces.append([csarg, randomname])
			lines.append(line)

		if replaces != []:

			#  必要ならばファイルを書き換える
			print('  -- replacing file: %s' % f)
			if options.save_original:
				tmpfname = mktmpfname(f, suffix)
				if rename(f, tmpfname) != 0:
					msg = 'rename failed: %s -> %s' % (f, tmpfname)
					sys.exit(1)
			outf = open(f, 'w', encoding='utf-8')
			for line in lines:
				outf.write(line)
			outf.close()

	#  対応表を中間ファイルに書き出す
	#
	intf = open(correspondence_file, 'w', encoding='utf-8')
	for line in replaces:
		intf.write('%s,%s\n' % (line[1], line[0]))
	intf.close()

#  中間ファイルに記録されているランダム文字列がhtmlファイルに現れたら
#  それを対応する引数情報に書き換える
#
def decode():

	#  対応表を中間ファイルから読む
	#
	replaces = []
	for line in open(correspondence_file, 'r', encoding='utf-8'):
		if line[-1] == '\n':
			line = line[:-1]
		replaces.append(line.split(','))

	#  htmlファイルを書き換えて引数情報を元に戻す
	#
	for f in fnames:
		if verbose:
			print('  decoding %s' % f)
		lines = []
		for line in open(f, 'r', encoding='utf-8'):
			p1 = line.find('<a href=')
			if p1 < 0:
				#  ファイル参照のない行
				for key,val in replaces:
					if line.find(key) >= 0:
						line = line.replace(key, val)
						break
				lines.append(line)
				continue

			p2 = line[p1:].find('>')
			if p2 < 0:
				#  anchorタグが閉じていない行
				#  ファイル参照は変換しない
				if verbose:
					print('    uncloded <a>: %s' % line[:-1])
				lines.append(line)
				continue

			#   <a href="...">text</a>
			#   textの部分だけ変換対象とする
			p2 = p1 + p2 + 1
			line1 = line[:p1]
			line2 = line[p1:p2]
			line3 = line[p2:]
			for key,val in replaces:
				if line3.find(key) >= 0:
					line3 = line3.replace(key, val)
					if verbose:
						print('    %s -> %s' % (key, val))
					line = line1 + line2 + line3
					break
			lines.append(line)

		if options.save_original:
			tmpfname = mktmpfname(f, suffix)
			if rename(f, tmpfname) != 0:
				msg = 'rename failed: %s -> %s' % (f, tmpfname)
				sys.exit(1)
		outf = open(f, 'w', encoding='utf-8')
		for line in lines:
			outf.write(line)
		outf.close()

#  指定された長さのランダム文字列を作成する（文字はseedの中から取り出す）
#
def random_str(seed, size):
	strlen = len(seed)
	rand = ''
	for i in range(size):
		n = strlen
		while n >= strlen:
			n = int(random.random() * 100)
		rand += seed[n]
	return rand

#  作業ファイル名を作成する
#
def mktmpfname(basename, suffix):
	tmpfname = '%s.%s' % (basename, suffix)
	if os.path.exists(tmpfname):
		return mktmpfname(tmpfname, suffix)
	return tmpfname

#  ファイル名を変更する
#
def rename(fm , to):
	if is_unix():
		cmnd = 'mv %s %s' % (fm, to)
	else:
		cmnd = 'rename %s %s' % (fm, to)
	proc = subprocess.Popen(cmnd, shell=True)
	rc = proc.wait()
	if not is_unix():
		rc = -(rc & 0b1000000000000000) | (rc & 0b0111111111111111)
	return rc

#  現在のOSを判定する
#
def is_unix():
	return True if os.name == 'posix' else False


# ----------------------------------------------------------------------
#  Main process.
#
if func == 'enc':
	# csname -> random string
	encode()
elif func == 'dec':
	# random string -> csname
	decode()

# ----------------------------------------------------------------------
#  End of process.
#
if verbose:
	print('%s: %s: done' % (prog, func))
sys.exit(0)

# end: csname_replace.py
