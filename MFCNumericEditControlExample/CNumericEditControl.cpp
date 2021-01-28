/*
	CNumericEditControl.h

	Simple replacement for MFC CEdit class that implements numeric decimal/hex/octal/binary input
	with clipboard and format conversion support. Hex input may optionally be prefixed with "0x"
	and octal may optionally prefixed with "0". The control does not use PreTranslateMessage()
	and can be used in both standard MFC applications and DLL projects that do not have a message
	loop. The current input mode is displayed using a "cue" or watermark text.
	
	MFC usage instructions:

	1. Add "CNumericEditControl.h" and "CNumericEditControl.cpp" to your MFC project
	2. If necessary, add common controls manifest (see "stdafx.h" in example project)
	3. #include "CNumericEditControl.h"
	4. Add edit control to your dialog
	5. Add control variable for the edit control	
	6. Change the control variable type from CEdit to CNumericEditControl
	7. Use the control as normal
	8. Use methods AsString() or AsValue() to access value
	9. If necessary, call ChangeMode() to change the display mode at runtime

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

#include "stdafx.h"
#include "CNumericEditControl.h"

// CNumericEditControl

//Private definitions
//
#define WM_DECMODE		WM_USER + 0x7F00
#define WM_HEXMODE		WM_USER + 0x7F01
#define WM_OCTMODE		WM_USER + 0x7F02
#define WM_BINMODE		WM_USER + 0x7F03

IMPLEMENT_DYNAMIC(CNumericEditControl, CEdit)

CNumericEditControl::CNumericEditControl()
{
	m_llInitialValue = VALUEINVALID;
	m_modeEx = EDisplayMode::DISPLAY_DEC;
}

CNumericEditControl::CNumericEditControl(EDisplayMode mode)
{
	m_llInitialValue = VALUEINVALID;
	m_modeEx = mode;
}

CNumericEditControl::CNumericEditControl(LONGLONG llInitialValue, EDisplayMode mode)
{
	m_llInitialValue = llInitialValue;
	m_modeEx = mode;
}

CNumericEditControl::~CNumericEditControl()
{
}

BEGIN_MESSAGE_MAP(CNumericEditControl, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CONTEXTMENU()
	ON_WM_CHAR()	
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

//Control lost focus
void CNumericEditControl::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
}

//Filter input to prevent invalid characters
void CNumericEditControl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{	
	//Ignore all but permitted characters
	BOOL bAllowed = false;

	CString sValue;
	GetWindowText(sValue);
	int nValueLength = sValue.GetLength();

	//Backspace always permitted
	if (nChar == VK_BACK)
		bAllowed = true;

	//Decimal input
	else if (m_modeEx == EDisplayMode::DISPLAY_DEC)
	{
		//0 not permitted if leading character in decimal mode (to avoid confusion with octal)
		if (nValueLength == 0 && nChar == '0')
		bAllowed = false;

		//0-9 otherwise always permitted
		else if (nChar >= '0' && nChar <= '9')
			bAllowed = true;
	}

	//Hex input
	else if (m_modeEx == EDisplayMode::DISPLAY_HEX)
	{
		//0 not permitted as second character in hex mode if leading character was 0 (to force 0x format)
		if (nValueLength == 1 && sValue[0] == L'0' && nChar == '0')
			bAllowed = false;

		//x and X permitted in hex mode for "0x" but only for second character
		else if (nValueLength == 1 && (nChar == 'x' || nChar == 'X'))
			bAllowed = true;

		//0-9 otherwise always permitted
		else if (nChar >= '0' && nChar <= '9')
			bAllowed = true;

		//a-f also permitted in hex mode
		else if (nChar >= 'a' && nChar <= 'f')
			bAllowed = true;

		//A-F also permitted in hex mode
		else if (nChar >= 'A' && nChar <= 'F')
			bAllowed = true;		
	}

	//Octal input
	else if (m_modeEx == EDisplayMode::DISPLAY_OCTAL)
	{
		//0 not permitted as second character in octal mode if leading character was 0 (to force leading 0 format)
		if (nValueLength == 1 && sValue[0] == L'0' && nChar == '0')
			bAllowed = false;
		
		//0-7 otherwise always permitted
		else if (nChar >= '0' && nChar <= '7')
			bAllowed = true;
	}

	//Binary input
	else if (m_modeEx == EDisplayMode::DISPLAY_BINARY)
	{		
		//0-1 always permitted
		if (nChar >= '0' && nChar <= '1')
			bAllowed = true;
	}		

	if (bAllowed)
		CEdit::OnChar(nChar, nRepCnt, nFlags);	
}

void CNumericEditControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//Assume key not handled
	BOOL bHandled = false;

	//Handle ^X, ^C and ^V for clipboard Cut/Copy/Paste
	if ((GetKeyState(VK_CONTROL) < 0) && !(GetKeyState(VK_SHIFT) < 0))
	{		
		if (nChar == 'x' || nChar == 'X')
		{			
			OnCommand(WM_CUT, 0);
			bHandled = true;
		}

		else if (nChar == 'c' || nChar == 'C')
		{
			OnCommand(WM_COPY, 0);
			bHandled = true;
		}

		else if (nChar == 'v' || nChar == 'V')
		{
			OnCommand(WM_PASTE, 0);
			bHandled = true;
		}
	}

	//Forward unhandled key to base class
	if (!bHandled)
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

CString CNumericEditControl::AsString(void)
{
	CString sValue;
	GetWindowText(sValue);
	return sValue;
}

BOOL CNumericEditControl::ParseValueInternal(LPCWSTR pszString, int nRadix, PLONGLONG pllResult)
{	
	/*
		MSDN:	strtoull returns the converted value, if any, or ULLONG_MAX on overflow. 
				strtoull returns 0 if no conversion can be performed. wcstoull returns values analogously to strtoull. 
				For both functions, errno is set to ERANGE if overflow or underflow occurs.
	*/

	//Ignore commas and spaces (common input from Windows Calculator application)
	CString sCookedString = pszString;
	sCookedString.Replace(L",", L"");
	sCookedString.Replace(L" ", L"");

	errno = 0;
	wchar_t *end = NULL;	
	LONGLONG llValue=wcstoull(sCookedString.GetString(), &end, nRadix);

	//Not a valid number
	if (end == sCookedString.GetString())
		return false;

	//Additional characters after numeric
	else if (L'\0' != *end)
		return false;

	//Value out of range
	else if ((LLONG_MIN == llValue || ULLONG_MAX == llValue) && ERANGE == errno)
		return false;

	*pllResult = llValue;
	return true;
}

LONGLONG CNumericEditControl::AsValue(void)
{
	CString sValue;
	GetWindowText(sValue);
	if (!sValue.GetLength())
		return VALUEINVALID;

	LONGLONG llValue = VALUEINVALID;
	if (m_modeEx == EDisplayMode::DISPLAY_DEC)
	{		
		if (!ParseValueInternal(sValue, 10, &llValue))
			return VALUEINVALID;
	}
	else if (m_modeEx == EDisplayMode::DISPLAY_HEX)
	{
		if (!ParseValueInternal(sValue, 16, &llValue))
			return VALUEINVALID;
	}	
	else if (m_modeEx == EDisplayMode::DISPLAY_OCTAL)
	{
		if (!ParseValueInternal(sValue, 8, &llValue))
			return VALUEINVALID;
	}
	else if (m_modeEx == EDisplayMode::DISPLAY_BINARY)
	{
		if (!ParseValueInternal(sValue, 2, &llValue))
			return VALUEINVALID;
	}
	
	return llValue;
}

void CNumericEditControl::SetString(CString sText)
{
	SetWindowText(sText);
}

void CNumericEditControl::SetValue(LONGLONG llNewValue)
{
	UpdateControl(llNewValue);
}

void CNumericEditControl::Empty(void)
{
	SetWindowText(L"");
}

//Init control
void CNumericEditControl::PreSubclassWindow()
{
	CEdit::PreSubclassWindow();
	UpdateControl(m_llInitialValue);
}

void CNumericEditControl::UpdateControl(LONGLONG llNewValue)
{
	//Set watermark
	if (m_modeEx == EDisplayMode::DISPLAY_DEC)
		SetCueBanner(L"Decimal", true);

	else if (m_modeEx == EDisplayMode::DISPLAY_HEX)
		SetCueBanner(L"Hex", true);

	else if (m_modeEx == EDisplayMode::DISPLAY_OCTAL)
		SetCueBanner(L"Octal", true);

	else if (m_modeEx == EDisplayMode::DISPLAY_BINARY)
		SetCueBanner(L"Binary", true);
	
	//Display formatted numeric value
	CString sText;
	if (llNewValue >= 0)
	{	
		if (m_modeEx == EDisplayMode::DISPLAY_DEC)
			sText.Format(L"%I64u", (ULONGLONG)llNewValue);

		else if (m_modeEx == EDisplayMode::DISPLAY_HEX)
			sText.Format(L"0x%I64x", (ULONGLONG)llNewValue);

		else if (m_modeEx == EDisplayMode::DISPLAY_OCTAL)
			sText.Format(L"0%I64o", (ULONGLONG)llNewValue);

		else if (m_modeEx == EDisplayMode::DISPLAY_BINARY)
		{			
			WCHAR szBuffer[_MAX_U64TOSTR_BASE2_COUNT];
			_ui64tow_s(llNewValue, szBuffer, _MAX_U64TOSTR_BASE2_COUNT, 2);
			sText = szBuffer;			
		}	
	}
	
	SetWindowText(sText);
}

void CNumericEditControl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	SetFocus();

	CString sValue;
	GetWindowText(sValue);
	
	//Create context menu. The current mode is checked
	//
	CMenu m_stMenuContext;
	m_stMenuContext.CreatePopupMenu();
	m_stMenuContext.InsertMenu(0, MF_BYPOSITION | (m_modeEx == EDisplayMode::DISPLAY_DEC ? MF_CHECKED : 0),    WM_DECMODE, L"Decimal");
	m_stMenuContext.InsertMenu(1, MF_BYPOSITION | (m_modeEx == EDisplayMode::DISPLAY_HEX ? MF_CHECKED : 0),    WM_HEXMODE, L"Hex");
	m_stMenuContext.InsertMenu(2, MF_BYPOSITION | (m_modeEx == EDisplayMode::DISPLAY_OCTAL ? MF_CHECKED : 0),  WM_OCTMODE, L"Octal");
	m_stMenuContext.InsertMenu(3, MF_BYPOSITION | (m_modeEx == EDisplayMode::DISPLAY_BINARY ? MF_CHECKED : 0), WM_BINMODE, L"Binary");
	m_stMenuContext.InsertMenu(4, MF_BYPOSITION | (sValue.GetLength() ? 0 : MF_GRAYED), WM_CUT, L"Cut");
	m_stMenuContext.InsertMenu(5, MF_BYPOSITION | (sValue.GetLength() ? 0 : MF_GRAYED), WM_COPY, L"Copy");
	m_stMenuContext.InsertMenu(6, MF_BYPOSITION | (GetClipboardText().GetLength() ? 0 : MF_GRAYED), WM_PASTE, L"Paste");
	m_stMenuContext.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON |TPM_RIGHTBUTTON, point.x, point.y, this);
}

BOOL CNumericEditControl::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		
		case WM_DECMODE:	//Flip to decimal mode
							ChangeMode(EDisplayMode::DISPLAY_DEC);
							return true;

		case WM_HEXMODE:	//Flip to hex mode
							ChangeMode(EDisplayMode::DISPLAY_HEX);
							return true;

		case WM_OCTMODE:	//Flip to octal mode
							ChangeMode(EDisplayMode::DISPLAY_OCTAL);
							return true;

		case WM_BINMODE:	//Flip to binary mode
							ChangeMode(EDisplayMode::DISPLAY_BINARY);
							return true;

		case WM_CUT:		//Cut text to clipboard
		case WM_COPY:		//Copy text to clipboard
							{
								CString sValue;
								GetWindowText(sValue);
					
								if (!sValue.GetLength())
									return true;
								
								size_t nLength = (sValue.GetLength() + 1) * sizeof(WCHAR);
								HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, nLength);
								if (hMem)
								{
									LPVOID lpMem = GlobalLock(hMem);
									if (lpMem)
									{
										memcpy(lpMem, sValue.GetString(), nLength);
										GlobalUnlock(hMem);

										if (OpenClipboard())
										{
											EmptyClipboard();
											SetClipboardData(CF_UNICODETEXT, hMem);
											CloseClipboard();
										}
									}
								}

								//Clear current value
								if (LOWORD(wParam) == WM_CUT)
								{
									SetWindowText(L"");
									UpdateControl();
								}
							}

							return true;

		case WM_PASTE:		//Paste text from clipboard							
							{
								CString sNewValue = GetClipboardText();
								if (!sNewValue.GetLength())
									return true;														

								//Parse text and determine if valid numeric value
								LONGLONG llValue = VALUEINVALID;
								if (m_modeEx == EDisplayMode::DISPLAY_DEC)
								{
									if (!ParseValueInternal(sNewValue, 10, &llValue))
										llValue = VALUEINVALID;
								}
								else if (m_modeEx == EDisplayMode::DISPLAY_HEX)
								{
									if (!ParseValueInternal(sNewValue, 16, &llValue))
										llValue = VALUEINVALID;
								}
								else if (m_modeEx == EDisplayMode::DISPLAY_OCTAL)
								{
									if (!ParseValueInternal(sNewValue, 8, &llValue))
										llValue = VALUEINVALID;
								}
								else if (m_modeEx == EDisplayMode::DISPLAY_BINARY)
								{
									if (!ParseValueInternal(sNewValue, 2, &llValue))
										llValue = VALUEINVALID;
								}
								
								//Used parsed value						
								UpdateControl(llValue);								
							}
							return true;
			
		default:			return CEdit::OnCommand(wParam, lParam);
			
	}
}

void CNumericEditControl::ChangeMode(EDisplayMode newMode)
{	
	//No change required
	if (m_modeEx == newMode)
		return;
	
	//Parse value and change mode
	LONGLONG llCurrentValue = AsValue();
	m_modeEx = newMode;	
	UpdateControl(llCurrentValue);
}

CString CNumericEditControl::GetClipboardText()
{
	CString sResult;

	if (OpenClipboard())
	{
		HANDLE hData = GetClipboardData(CF_UNICODETEXT);
		if (hData)
		{
			sResult = (LPCWSTR)GlobalLock(hData);
			GlobalUnlock(hData);
		}

		CloseClipboard();
	}

	return sResult;
}