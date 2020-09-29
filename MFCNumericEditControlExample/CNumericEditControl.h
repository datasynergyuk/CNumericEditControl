#pragma once

/*
	CNumericEditControl.h

	Simple replacement for MFC CEdit class that implements numeric decimal/hex/octal/binary input
	with clipboard and format conversion support. Hex input may optionally be prefixed with "0x"
	and octal may optionally prefixed with "0". The control does not use PreTranslateMessage()
	and can be used in both standard MFC applications and DLL projects that do not have a message
	loop. The current input mode is displayed using a "cue" or watermark text.

	See CNumericEditControl.cpp for usage instructions

	MIT License for CNumericEditControl:

	Copyright (c) 2019-2020 Data Synergy UK Ltd

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#pragma once

// CNumericEditControl

class CNumericEditControl : public CEdit
{
	DECLARE_DYNAMIC(CNumericEditControl)

public:
	static constexpr auto VALUEINVALID = -1;
	enum class EDisplayMode : WORD 
	{
		DISPLAY_DEC,
		DISPLAY_HEX,
		DISPLAY_OCTAL,
		DISPLAY_BINARY,
	};

	CNumericEditControl();
	CNumericEditControl(EDisplayMode mode);
	CNumericEditControl(LONGLONG llInitialValue, EDisplayMode mode);	
	virtual ~CNumericEditControl();

	CString AsString(void);
	LONGLONG AsValue(void);
	void SetString(CString sText);
	void ChangeMode(EDisplayMode newMode);
	void Empty(void);

private:
	EDisplayMode m_modeEx;
	LONGLONG m_llInitialValue;

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void UpdateControl(LONGLONG llNewValue = VALUEINVALID);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL ParseValueInternal(LPCWSTR pszString, int nRadix, PLONGLONG pllResult);	
	CString GetClipboardText();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};

