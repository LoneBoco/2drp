/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ShellFileInterface.h"

#include "client/game/Game.h"
#include "engine/filesystem/FileSystem.h"

#include <stdio.h>


namespace tdrp::ui
{

ShellFileInterface::ShellFileInterface()
{
}

ShellFileInterface::~ShellFileInterface()
{
	for (auto& [id, file] : m_files)
	{
		file->Close();
	}

	m_files.clear();
}

// Opens a file.
Rml::FileHandle ShellFileInterface::Open(const Rml::String& path)
{
	auto file = Game->Server.FileSystem.GetFile(path);
	if (!file) return 0;

	auto id = file->Crc32();
	m_files[id] = file;

	return id;
}

// Closes a previously opened file.
void ShellFileInterface::Close(Rml::FileHandle file)
{
	auto it = m_files.find(file);
	if (it == std::end(m_files))
		return;

	it->second->Close();
	m_files.erase(it);
}

// Reads data from a previously opened file.
size_t ShellFileInterface::Read(void* buffer, size_t size, Rml::FileHandle file)
{
	auto it = m_files.find(file);
	if (it == std::end(m_files))
	{
		static_cast<char*>(buffer)[0] = '\0';
		return 0;
	}

	auto& f = it->second;
	return f->ReadIntoBuffer(reinterpret_cast<uint8_t*>(buffer), size);
}

// Seeks to a point in a previously opened file.
bool ShellFileInterface::Seek(Rml::FileHandle file, long offset, int origin)
{
	auto it = m_files.find(file);
	if (it == std::end(m_files))
		return false;

	std::ios_base::seekdir dir{ std::ios_base::beg };
	switch (origin)
	{
	case SEEK_CUR:
		dir = std::ios_base::cur;
		break;
	case SEEK_END:
		dir = std::ios_base::end;
	}

	it->second->SetReadPosition(offset, dir);
	return true;
}

// Returns the current position of the file pointer.
size_t ShellFileInterface::Tell(Rml::FileHandle file)
{
	auto it = m_files.find(file);
	if (it == std::end(m_files))
		return 0;

	return it->second->GetReadPosition();
}

} // end namespace tdrp::ui
