//	Copyright (c) 2011-2015 by Artem A. Gevorkyan (gevorkyan.org)
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.

#include "ProfilerSink.h"

#include "function_list.h"
#include "object_lock.h"
#include "ProfilerMainDialog.h"
#include "symbol_resolver.h"

namespace std
{
	using tr1::bind;
}

using namespace std;

namespace micro_profiler
{
	namespace
	{
		typedef micro_profiler::ProfilerFrontend _ProfilerFrontend;

		OBJECT_ENTRY_AUTO(CLSID_ProfilerFrontend, _ProfilerFrontend);

		void disconnect(IUnknown *object)
		{	::CoDisconnectObject(object, 0);	}
	}

	ProfilerFrontend::ProfilerFrontend()
	{	}

	ProfilerFrontend::~ProfilerFrontend()
	{	}

	void ProfilerFrontend::FinalRelease()
	{
		_dialog.reset();
		_statistics.reset();
		_symbols.reset();
	}

	STDMETHODIMP ProfilerFrontend::Initialize(const ProcessInitializationData *process)
	{
		wchar_t filename[MAX_PATH] = { 0 }, extension[MAX_PATH] = { 0 };

		_wsplitpath_s(process->ExecutablePath, 0, 0, 0, 0, filename, MAX_PATH, extension, MAX_PATH);
	
		_symbols = symbol_resolver::create();
		_statistics = functions_list::create(process->TicksResolution, _symbols);
		_dialog.reset(new ProfilerMainDialog(_statistics, wstring(filename) + extension));
		_dialog->ShowWindow(SW_SHOW);
		_closed_connected = _dialog->Closed += std::bind(&disconnect, this);

		lock(_dialog);
		return S_OK;
	}

	STDMETHODIMP ProfilerFrontend::LoadImages(long count, const ImageLoadInfo *images)
	{
		for (; count; --count, ++images)
			_symbols->add_image(images->Path, reinterpret_cast<const void *>(images->Address));
		return S_OK;
	}

	STDMETHODIMP ProfilerFrontend::UpdateStatistics(long count, const FunctionStatisticsDetailed *statistics)
	{
		_statistics->update(statistics, count);
		return S_OK;
	}

	STDMETHODIMP ProfilerFrontend::UnloadImages(long count, const long long *image_addresses)
	{
		for (; count; --count, ++image_addresses)
		{	}
		return S_OK;
	}
}
