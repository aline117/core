#include "CCompiledStyle.h"

#include <cwctype>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iterator>
#include <map>

#include <iostream>
#include "../../../../../DesktopEditor/common/File.h"
#include "StaticFunctions.h"
#include "ConstValues.h"

namespace NSCSS
{
    typedef std::map<std::wstring, std::wstring>::const_iterator styles_iterator;

    CCompiledStyle::CCompiledStyle() {}

    CCompiledStyle::CCompiledStyle(const std::map<std::wstring, std::wstring>& mStyle) : m_mStyle(mStyle) {}

    CCompiledStyle::CCompiledStyle(const CCompiledStyle& oStyle) :
        m_mStyle(oStyle.m_mStyle), m_arParentsStyles(oStyle.m_arParentsStyles), m_sId(oStyle.m_sId) {}

    CCompiledStyle::~CCompiledStyle()
    {
        m_mStyle.         clear();
        m_arParentsStyles.clear();
    }


    CCompiledStyle& CCompiledStyle::operator+= (const CCompiledStyle &oElement)
    {
        for (const std::map<std::wstring, std::wstring>::value_type& oItem : oElement.m_mStyle)
        {
            if (!oItem.second.empty() && oItem.second != L"inherit")
            {
                m_mStyle[oItem.first] = oItem.second;
            }
        }
        return *this;
    }

    CCompiledStyle& CCompiledStyle::operator= (const CCompiledStyle &oElement)
    {
        m_mStyle = oElement.m_mStyle;
        m_sId    = oElement.m_sId;
        m_arParentsStyles = oElement.m_arParentsStyles;
        return *this;
    }

    bool CCompiledStyle::operator== (const CCompiledStyle& oStyle) const
    {
        if (oStyle.m_mStyle.size() != m_mStyle.size() ||
            oStyle.m_arParentsStyles.size() != m_arParentsStyles.size() ||
            oStyle.GetId()[0] != m_sId[0])
            return false;

        return GetId()[0]        == oStyle.GetId()[0]        &&
               m_arParentsStyles == oStyle.m_arParentsStyles &&
               m_mStyle          == oStyle.m_mStyle;
    }

    std::wstring CCompiledStyle::GetStyleW() const
    {
        return std::accumulate(m_mStyle.begin(), m_mStyle.end(), std::wstring(),
            [] (std::wstring& sRes, const std::map<std::wstring, std::wstring>::value_type& oIter) { return sRes += oIter.first + L":" + oIter.second + L";"; });
    }

    std::map<std::wstring, std::wstring>* CCompiledStyle::GetStyleMap()
    {
        return &m_mStyle;
    }

//    bool CCompiledStyle::operator<(const CCompiledStyle &oElement) const
//    {
//        return m_sId < oElement.m_sId;
//    }

    /*
    bool CCompiledStyle::operator!=(const CCompiledStyle &oElement) const
    {
        if (*this > oElement)
            return false;

        if (*this < oElement)
            return false;

        return true;
    }

    bool CCompiledStyle::operator>(const CCompiledStyle &oElement) const
    {
        return GetWeidth() > oElement.GetWeidth();
    }

    std::string CCompiledStyle::GetStyle() const
    {
        std::wstring sStyle = GetStyleW();
        return U_TO_UTF8(sStyle);
    }

    size_t CCompiledStyle::GetSize() const
    {
        return m_mStyle.size();
    }
    */

    bool CCompiledStyle::Empty() const
    {
        return m_mStyle.empty();
    }

    /*
    void CCompiledStyle::Clear()
    {
        m_mStyle.clear();
        m_sId.clear();
        m_arParentsStyles.clear();
    }
    */

    void CCompiledStyle::AddPropSel(const std::wstring& sProperty, const std::wstring& sValue, const bool& bHardMode)
    {
        if (m_mStyle[sProperty].find(L'!') != std::wstring::npos)
            return;

        std::wstring sNetValue = sValue;
        if (sNetValue.substr(0, 3) == L"rgb")
            sNetValue = NSCSS::NS_STATIC_FUNCTIONS::ConvertRgbToHex(sNetValue);

        if (!bHardMode)
            m_mStyle.emplace(sProperty, sNetValue);
        else
            m_mStyle[sProperty] = sNetValue;
    }

    void CCompiledStyle::AddStyle(const std::map<std::wstring, std::wstring>& mStyle, const bool& bHardMode)
    {
        for (const std::pair<std::wstring, std::wstring> pPropertie : mStyle)
        {
            if (m_mStyle[pPropertie.first].empty() || bHardMode)
                m_mStyle[pPropertie.first] = pPropertie.second;
            else if (!bHardMode || m_mStyle[pPropertie.first].find(L'!') != std::wstring::npos)
                continue;
        }
    }


    void CCompiledStyle::AddStyle(const std::wstring& sStyle, const bool& bHardMode)
    {
        if (sStyle.empty())
            return;

        const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsWithSigns(sStyle, L" :;");

        std::wstring sProperty, sValue;

        for (std::vector<std::wstring>::const_iterator iWord = arWords.begin(); iWord != arWords.end(); ++iWord)
            if ((*iWord).back() == L':')
            {
                sProperty = *iWord;
                sProperty.pop_back();
            }
            else
            {
                sValue += *iWord;
                if ((*iWord).back() == L';')
                {
                    sValue.pop_back();
                    std::transform(sProperty.begin(), sProperty.end(), sProperty.begin(), tolower);
                    std::transform(sValue.begin(), sValue.end(), sValue.begin(), tolower);
                    AddPropSel(sProperty, sValue, bHardMode);
                    sProperty.clear();
                    sValue.clear();
                }
            }

        if (!sProperty.empty() && !sValue.empty())
            AddPropSel(sProperty, sValue, bHardMode);
    }

    void CCompiledStyle::SetStyle(const std::map<std::wstring, std::wstring> &mStyle)
    {
        m_mStyle = mStyle;
    }

    void CCompiledStyle::AddParent(const std::wstring& sParentName)
    {
        if (!sParentName.empty())
            m_arParentsStyles.push_back(sParentName);
    }

    std::vector<std::wstring> CCompiledStyle::GetParentsName() const
    {
        return m_arParentsStyles;
    }

    void CCompiledStyle::SetID(const std::wstring& sId)
    {
        m_sId = sId;
    }

    std::wstring CCompiledStyle::GetId() const
    {
        return m_sId;
    }

    /*
    const std::map<std::wstring, std::wstring>::iterator& CCompiledStyle::GetBegin()
    {
        return m_mStyle.begin();
    }

    const std::map<std::wstring, std::wstring>::iterator& CCompiledStyle::GetEnd()
    {
        return m_mStyle.end();
    }

    double CCompiledStyle::GetWeidth() const
    {
        double dWidth = 0.0;
        dWidth = std::accumulate(m_mStyle.begin(), m_mStyle.end(), dWidth,
            [] (double dW, const std::pair<std::wstring, std::wstring>& sValue) { return dW + sValue.first.length() + sValue.second.length(); });
        dWidth = std::accumulate(m_arParentsStyles.begin(), m_arParentsStyles.end(), dWidth,
            [] (double dW, const std::wstring& sValue) { return dW + sValue.length() / 2; });
        return dWidth;
    }
    */

    /* FONT */

        std::wstring CCompiledStyle::GetFont() const
        {
            styles_iterator oFont = m_mStyle.find(L"font");
            if (oFont != m_mStyle.end())
                return oFont->second;

            std::wstring sValue = GetFontStyle()   + L" " +
                                  GetFontVariant() + L" " +
                                  GetFontWeight()  + L" " +
                                  GetFontSize()    + L"/" +
                                  GetLineHeight()  + L" " +
                                  GetFontFamily();

            return sValue.length() == 5 ? std::wstring() : sValue;
        }

        std::wstring CCompiledStyle::GetFontFamily() const
        {
            styles_iterator oFontFamily = m_mStyle.find(L"font-family");

            if (oFontFamily != m_mStyle.end())
            {
                const std::wstring& sFont = oFontFamily->second;
                size_t nLeftQuote = sFont.find_first_of(L"'\"");
                while (nLeftQuote != std::wstring::npos)
                {
                    const size_t nRightQuote = sFont.find_first_of(L"'\"", nLeftQuote + 1);
                    const size_t nNewLeftQuote = sFont.find_first_of(L"'\"", nRightQuote + 1);
                    if (nNewLeftQuote == std::wstring::npos)
                        return sFont.substr(nLeftQuote, nRightQuote - nLeftQuote + 1);
                    nLeftQuote = nNewLeftQuote;
                }
            }

            std::wstring sFont;
            styles_iterator oFont = m_mStyle.find(L"font");

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            size_t nLeftQuote = sFont.find_first_of(L"'\"");
            while (nLeftQuote != std::wstring::npos)
            {
                const size_t nRightQuote = sFont.find_first_of(L"'\"", nLeftQuote + 1);
                const size_t nNewLeftQuote = sFont.find_first_of(L"'\"", nRightQuote + 1);
                if (nNewLeftQuote == std::wstring::npos)
                    return sFont.substr(nLeftQuote, nRightQuote - nLeftQuote + 1);
                nLeftQuote = nNewLeftQuote;
            }

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetFontSize() const
        {
            styles_iterator oFontSize = m_mStyle.find(L"font-size");

            if (oFontSize != m_mStyle.end())
                return oFontSize->second;

            std::wstring sFont;

            styles_iterator oFont = m_mStyle.find(L"font");

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsW(sFont);

            std::wstring sValue;

            for (const std::wstring& sWord : arWords)
                if (isdigit(sWord[0]))
                    sValue = sWord;
                else if (sWord == L"/")
                    return sValue;

            return sValue;
        }

        std::wstring CCompiledStyle::GetFontSizeAdjust() const
        {
            styles_iterator oFontSizeAdj = m_mStyle.find(L"font-size-adjust");
            return oFontSizeAdj != m_mStyle.end() ? oFontSizeAdj->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetFontStretch() const
        {
            styles_iterator oFontStretch = m_mStyle.find(L"font-stretch");
            return oFontStretch != m_mStyle.end() ? oFontStretch->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetFontStyle() const
        {
            styles_iterator oFontStyle = m_mStyle.find(L"font-style");

            if (oFontStyle != m_mStyle.end())
                return oFontStyle->second;

            std::wstring sFont;

            styles_iterator oFont = m_mStyle.find(L"font");

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsW(sFont);

            for (const std::wstring& sWord : arWords)
                if (sWord == L"italic" || sWord == L"oblique")
                    return sWord;

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetFontVariant() const
        {
            styles_iterator oFontVariant = m_mStyle.find(L"font-variant");

            if (oFontVariant != m_mStyle.end())
                return oFontVariant->second;

            styles_iterator oFont = m_mStyle.find(L"font");
            std::wstring sFont;

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsW(sFont);

            for (const std::wstring& sWord : arWords)
                if (sWord == L"small-caps")
                    return sWord;

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetFontWeight() const
        {
            styles_iterator oFontWeight = m_mStyle.find(L"font-weight");

            if (oFontWeight != m_mStyle.end())
                return oFontWeight->second;

            styles_iterator oFont = m_mStyle.find(L"font");
            std::wstring sFont;

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsW(sFont);

            for (const std::wstring& sWord : arWords)
                if (sWord == L"bold" || sWord == L"bolder" || L"lighter" || (sWord.length() == 3 && isdigit(sWord[0]) && isdigit(sWord[1]) && isdigit(sWord[2])))
                    return std::wstring(L"bold");

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetLineHeight() const
        {
            styles_iterator oLineHeight = m_mStyle.find(L"line-height");

            if (oLineHeight != m_mStyle.end())
                return oLineHeight->second;

            std::wstring sFont;

            styles_iterator oFont = m_mStyle.find(L"font");

            if (oFont != m_mStyle.end())
                sFont = oFont->second;

            if (sFont.empty())
                return std::wstring();

            const std::vector<std::wstring> arWords = NS_STATIC_FUNCTIONS::GetWordsW(sFont);

            std::wstring sValue;

            for (size_t i = 0; i < arWords.size() - 1; ++i)
                if (arWords[i] == L"/" && i < arWords.size() - 2)
                    return arWords[i + 1];

            return std::wstring();

        }

        std::vector<std::wstring> CCompiledStyle::GetFontNames() const
        {
            const std::wstring sFontFamily = GetFontFamily();
            if (sFontFamily.empty())
                return std::vector<std::wstring>();


            size_t posFirst = sFontFamily.find_first_of(L"'\"");
                if (posFirst == std::wstring::npos)
                    return std::vector<std::wstring>({sFontFamily});

            std::vector<std::wstring> arWords;
            while (posFirst != std::wstring::npos)
            {
                const size_t& posSecond = sFontFamily.find_first_of(L"'\"", posFirst + 1);
                arWords.push_back(sFontFamily.substr(posFirst, posSecond - posFirst + 1));
                posFirst = sFontFamily.find_first_of(L"'\"", posSecond + 1);
            }

            return arWords;
        }

        std::vector<std::wstring> CCompiledStyle::GetFontNames2(const std::wstring& sFontFamily) const
        {
            if (sFontFamily.empty())
                return std::vector<std::wstring>();

            size_t posFirst = sFontFamily.find_first_of(L"'\"");
            if (posFirst == std::wstring::npos)
                return std::vector<std::wstring>({sFontFamily});

            std::vector<std::wstring> arWords;
            while (posFirst != std::wstring::npos)
            {
                const size_t& posSecond = sFontFamily.find_first_of(L"'\"", posFirst + 1);
                arWords.push_back(sFontFamily.substr(posFirst, posSecond - posFirst + 1));
                posFirst = sFontFamily.find_first_of(L"'\"", posSecond + 1);
            }
            return arWords;
        }

    /* MARGIN */
        std::wstring CCompiledStyle::GetMargin() const
        {
            styles_iterator oMargin = m_mStyle.find(L"margin");
            if (oMargin != m_mStyle.end())
                return oMargin->second;

            const std::wstring& sTop    = GetMarginTop();
            const std::wstring& sLeft   = GetMarginLeft();
            const std::wstring& sRight  = GetMarginRight();
            const std::wstring& sBottom = GetMarginBottom();

            if ((sTop == sLeft) && (sLeft == sRight) && (sRight == sBottom))
                return sTop;

            if ((sTop == sBottom) && (sLeft == sRight))
                return  sTop + L" " + sLeft;

            if (sLeft == sRight)
                return sTop + L" " + sLeft + L" " + sBottom;

            return sTop + L" " + sRight + L" " + sBottom + L" " + sLeft;

            return std::wstring();
        }

        std::vector<std::wstring> CCompiledStyle::GetMargins() const
        {
            styles_iterator oMargin = m_mStyle.find(L"margin");

            std::vector<std::wstring> sRes(4);

            std::wstring sMargin;
            if (oMargin != m_mStyle.end())
                sMargin = oMargin->second;

            std::wstring sTop    = GetMarginTop2();
            std::wstring sBottom = GetMarginBottom2();
            std::wstring sLeft   = GetMarginLeft2  ();
            std::wstring sRight  = GetMarginRight2 ();

            if(!sMargin.empty())
            {
                std::vector<std::wstring> arValues;
                size_t pre = 0;
                size_t find = sMargin.find(' ');
                while(find != std::wstring::npos)
                {
                    arValues.push_back(sMargin.substr(pre, find - pre));
                    pre = find + 1;
                    find = sMargin.find(' ', pre);
                }
                if(pre < sMargin.size())
                    arValues.push_back(sMargin.substr(pre));

                if(sTop.empty())
                    sTop = arValues[0];
                if(sLeft.empty())
                    sLeft = arValues.size() == 4 ? arValues[3] : (arValues.size() > 1 ? arValues[1] : arValues[0]);
                if(sRight.empty())
                    sRight = arValues.size() > 1 ? arValues[1] : arValues[0];
                if(sBottom.empty())
                    sBottom = arValues.size() > 2 ? arValues[2] : arValues[0];
            }
            sRes[0] = sTop;
            sRes[1] = sRight;
            sRes[2] = sBottom;
            sRes[3] = sLeft;
            return sRes;
        }

        std::wstring CCompiledStyle::GetMarginTop2() const
        {
            styles_iterator oMarginTop = m_mStyle.find(L"margin-top");
            if (oMarginTop != m_mStyle.end())
                return oMarginTop->second;

            const std::wstring& sMarginBlockStart = GetMarginBlockStart();
            return sMarginBlockStart.empty() ? std::wstring() : sMarginBlockStart;
        }

        std::wstring CCompiledStyle::GetMarginTop() const
        {
            styles_iterator oMarginTop = m_mStyle.find(L"margin-top");

            if (oMarginTop != m_mStyle.end())
                return oMarginTop->second;

            const std::wstring& sMarginBlockStart = GetMarginBlockStart();

            if (!sMarginBlockStart.empty())
                return sMarginBlockStart;

            std::wstring sMargin;
            styles_iterator oMargin = m_mStyle.find(L"margin");

            if (oMargin != m_mStyle.end())
                sMargin = oMargin->second;
            else
                return std::wstring();

            if (sMargin.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sMargin)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }
            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (!arValues.empty())
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginBlockStart() const
        {
            styles_iterator oMarginBlockStart = m_mStyle.find(L"margin-block-start");
            return oMarginBlockStart != m_mStyle.end() ? oMarginBlockStart->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginLeft2() const
        {
            styles_iterator oMarginLeft = m_mStyle.find(L"margin-left");
            return oMarginLeft != m_mStyle.end() ? oMarginLeft->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginLeft() const
        {
            styles_iterator oMarginLeft = m_mStyle.find(L"margin-left");

            if (oMarginLeft != m_mStyle.end())
                return oMarginLeft->second;

            std::wstring sValue;
            styles_iterator oMargin = m_mStyle.find(L"margin");

            if (oMargin != m_mStyle.end())
                sValue = oMargin->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }
            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() == 4)
                return arValues[3];
            else if (arValues.size() > 1)
                return arValues[1];
            else if (arValues.size() == 1)
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginRight2() const
        {
            styles_iterator oMarginRight = m_mStyle.find(L"margin-right");

            if (oMarginRight != m_mStyle.end())
                return oMarginRight->second;

            return L"";
        }

        std::wstring CCompiledStyle::GetMarginRight() const
        {
            styles_iterator oMarginRight = m_mStyle.find(L"margin-right");

            if (oMarginRight != m_mStyle.end())
                return oMarginRight->second;

            std::wstring sValue;
            styles_iterator oMargin = m_mStyle.find(L"margin");

            if (oMargin != m_mStyle.end())
                sValue = oMargin->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }

            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() > 1)
                return arValues[1];
            else if (arValues.size() == 1)
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginBottom2() const
        {
            styles_iterator oMarginBottom = m_mStyle.find(L"margin-bottom");
            if (oMarginBottom != m_mStyle.end())
                return oMarginBottom->second;

            const std::wstring& sMarginBlockEnd = GetMarginBlockEnd();
            return sMarginBlockEnd.empty() ? std::wstring() : sMarginBlockEnd;
        }

        std::wstring CCompiledStyle::GetMarginBottom() const
        {
            styles_iterator oMarginBottom = m_mStyle.find(L"margin-bottom");

            if (oMarginBottom != m_mStyle.end())
                return oMarginBottom->second;

            const std::wstring& sMarginBlockEnd = GetMarginBlockEnd();

            if (!sMarginBlockEnd.empty())
                return sMarginBlockEnd;

            std::wstring sValue;
            styles_iterator oMargin = m_mStyle.find(L"margin");

            if (oMargin != m_mStyle.end())
                sValue = oMargin->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }
            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() > 2)
                return arValues[2];
            else if (arValues.size() > 0)
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetMarginBlockEnd() const
        {
            styles_iterator oMarginBlockEnd = m_mStyle.find(L"margin-block-end");
            return oMarginBlockEnd != m_mStyle.end() ? oMarginBlockEnd->second : std::wstring();
        }

    /* PADDING */
        std::wstring CCompiledStyle::GetPadding() const
        {
            styles_iterator oPadding = m_mStyle.find(L"padding");

            if (oPadding != m_mStyle.end())
                return oPadding->second;

            const std::wstring& sTop    = GetPaddingTop();
            const std::wstring& sLeft   = GetPaddingLeft();
            const std::wstring& sRight  = GetPaddingRight();
            const std::wstring& sBottom = GetPaddingBottom();

            if ((sTop == sLeft) && (sLeft == sRight) && (sRight == sBottom))
                return sTop;

            if ((sTop == sBottom) && (sLeft == sRight))
                return  sTop + L" " + sLeft;

            if (sLeft == sRight)
                return sTop + L" " + sLeft + L" " + sBottom;

            return sTop + L" " + sRight + L" " + sBottom + L" " + sLeft;
        }

        std::wstring CCompiledStyle::GetPaddingTop() const
        {
            styles_iterator oPaddingTop = m_mStyle.find(L"padding-top");

            if (oPaddingTop != m_mStyle.end())
                return oPaddingTop->second;

            std::wstring sValue;
            styles_iterator oPadding = m_mStyle.find(L"padding");

            if (oPadding != m_mStyle.end())
                sValue = oPadding->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }
            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (!arValues.empty())
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetPaddingLeft() const
        {
            styles_iterator oPaddingLeft = m_mStyle.find(L"padding-left");

            if (oPaddingLeft != m_mStyle.end())
                return oPaddingLeft->second;

            std::wstring sValue;
            styles_iterator oPadding = m_mStyle.find(L"padding");

            if (oPadding != m_mStyle.end())
                sValue = oPadding->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }

            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() == 4)
                return arValues[3];
            else if (arValues.size() > 1)
                return arValues[1];
            else if (arValues.size() == 1)
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetPaddingRight() const
        {
            styles_iterator
                  oPaddingRight = m_mStyle.find(L"padding-right");

            if (oPaddingRight != m_mStyle.end())
                return oPaddingRight->second;

            std::wstring sValue;
            styles_iterator oPadding = m_mStyle.find(L"padding");

            if (oPadding != m_mStyle.end())
                sValue = oPadding->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }

            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() > 1)
                return arValues[1];
            else if (arValues.size() == 1)
                return arValues[0];

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetPaddingBottom() const
        {
            styles_iterator oPaddingBottom = m_mStyle.find(L"padding-bottom");

            if (oPaddingBottom != m_mStyle.end())
                return oPaddingBottom->second;

            std::wstring sValue;
            styles_iterator oPadding = m_mStyle.find(L"padding");

            if (oPadding != m_mStyle.end())
                sValue = oPadding->second;
            else
                return std::wstring();

            if (sValue.empty())
                return std::wstring();

            std::vector<std::wstring> arValues;

            std::wstring sTemp;

            for (const wchar_t& wc : sValue)
            {
                sTemp += wc;

                if (wc == L' ')
                {
                    arValues.push_back(sTemp);
                    sTemp.clear();
                }
            }

            if (!sTemp.empty())
                arValues.push_back(sTemp);

            if (arValues.size() > 2)
                return arValues[2];
            else if (arValues.size() > 0)
                return arValues[0];

            return std::wstring();
        }

    /*  SPACING */
        std::wstring CCompiledStyle::GetLetterSpacing() const
        {
            styles_iterator oLetterSpacing = m_mStyle.find(L"letter-spacing");

            if (oLetterSpacing != m_mStyle.end())
                return oLetterSpacing->second;

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetWordSpacing() const
        {
            styles_iterator oWordSpacing = m_mStyle.find(L"word-spacing");

            if (oWordSpacing != m_mStyle.end())
                return oWordSpacing->second;

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetBorderSpacing() const
        {
            styles_iterator oBorderSpacing = m_mStyle.find(L"border-spacing");

            if (oBorderSpacing != m_mStyle.end())
                return oBorderSpacing->second;

            return std::wstring();
        }

    /* COLOR */
        std::wstring CCompiledStyle::GetTextDecorationColor() const
        {
            styles_iterator oTextDecorationColor = m_mStyle.find(L"text-decoration-color");

            if (oTextDecorationColor != m_mStyle.end())
                return oTextDecorationColor->second;

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetBackgroundColor() const
        {
            styles_iterator oBackgroundColor = m_mStyle.find(L"background-color");

            if (oBackgroundColor != m_mStyle.end())
            {
                std::wstring sColor = oBackgroundColor->second;

                if (sColor[0] == L'#')
                {
                    if (sColor.length() == 7)
                        return sColor.substr(1, 7);
                    else if (sColor.length() == 4)
                    {
                        std::wstring sRetColor;
                        sRetColor += sColor[1];
                        sRetColor += sColor[1];
                        sRetColor += sColor[2];
                        sRetColor += sColor[2];
                        sRetColor += sColor[3];
                        sRetColor += sColor[3];
                        return sRetColor;
                    }
                    else
                        return L"auto";
                }

                std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                if (oHEX != NS_CONST_VALUES::mColors.end())
                    return oHEX->second;
            }
            const std::wstring& sBackground = GetBackground();

            if (sBackground.empty())
                return std::wstring();

            const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBackground);

            for (std::wstring sColor : arWords)
            {
                if (sColor[0] == L'#')
                {
                    if (sColor.length() == 7)
                        return sColor.substr(1, 7);
                    else if (sColor.length() == 4)
                    {
                        std::wstring sRetColor;
                        sRetColor += sColor[1];
                        sRetColor += sColor[1];
                        sRetColor += sColor[2];
                        sRetColor += sColor[2];
                        sRetColor += sColor[3];
                        sRetColor += sColor[3];
                        return sRetColor;
                    }
                    else
                        return L"auto";
                }

                std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                if (oHEX != NS_CONST_VALUES::mColors.end())
                    return oHEX->second;
            }

            return std::wstring();
        }

        std::wstring CCompiledStyle::GetColor() const
        {
            styles_iterator oColor = m_mStyle.find(L"color");
            if (oColor == m_mStyle.end())
                return std::wstring();

            std::wstring sColor = oColor->second;

            if (sColor[0] == L'#')
            {
                if (sColor.length() == 7)
                    return sColor.substr(1, 7);
                else if (sColor.length() == 4)
                {
                    std::wstring sRetColor;
                    sRetColor = sColor[1] + sColor[1] + sColor[2] + sColor[2] + sColor[3] + sColor[3];
                    return sRetColor;
                }
                else
                    return L"";
            }

            std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

            styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

            if (oHEX != NS_CONST_VALUES::mColors.end())
                return oHEX->second;

            return L"";
        }

        std::wstring CCompiledStyle::GetOutlineColor() const
        {
            styles_iterator oOutlineColor = m_mStyle.find(L"outline-color");
            return oOutlineColor != m_mStyle.end() ? oOutlineColor->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetColumnRuleColor() const
        {
            styles_iterator oColumnRuleColor = m_mStyle.find(L"column-rule-color");
            return oColumnRuleColor != m_mStyle.end() ? oColumnRuleColor->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetBackground() const
        {
            styles_iterator oBackground = m_mStyle.find(L"background");
            return oBackground != m_mStyle.end() ? oBackground->second : std::wstring();
        }

    /* TEXT */
        std::wstring CCompiledStyle::GetTextAlign() const
        {
            styles_iterator oTextAlign = m_mStyle.find(L"text-align");
            return oTextAlign != m_mStyle.end() ? oTextAlign->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetTextIndent() const
        {
            styles_iterator oTextIndent = m_mStyle.find(L"text-indent");
            return oTextIndent != m_mStyle.end() ? oTextIndent->second : std::wstring();
        }

        std::wstring CCompiledStyle::GetTextDecoration() const
        {
            styles_iterator oTextDecoration = m_mStyle.find(L"text-decoration");

            if (oTextDecoration != m_mStyle.end())
            {
                if (oTextDecoration->second == L"underline")
                    return std::wstring(L"single");
                else if (oTextDecoration->second == L"none")
                    return oTextDecoration->second;
            }

            return L"";

//            return oTextDecoration != m_mStyle.end() ? oTextDecoration->second : std::wstring();
        }

        /* BORDER */

            std::wstring CCompiledStyle::GetBorder() const
            {
                styles_iterator oBorder = m_mStyle.find(L"border");

                if (oBorder != m_mStyle.end() && oBorder->second != L"none")
                    return oBorder->second;
                else
                {
                    styles_iterator oMsoBorder = m_mStyle.find(L"mso-border-alt");
                    if (oMsoBorder != m_mStyle.end() && oMsoBorder->second != L"none")
                        return oMsoBorder->second;
                }

                const std::wstring& sBorderWidth = GetBorderWidth();
                const std::wstring& sBorderStyle = GetBorderStyle();
                const std::wstring& sBorderColor = GetBorderColor();

                if (!sBorderWidth.empty() && !sBorderStyle.empty() && !sBorderColor.empty())
                    return sBorderWidth + L" " + sBorderStyle + L" " + sBorderColor;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderWidth() const
            {
                styles_iterator oBorderWidth = m_mStyle.find(L"border-width");

                if (oBorderWidth != m_mStyle.end())
                    return oBorderWidth->second;

                styles_iterator oBorder = m_mStyle.find(L"border");

                std::wstring sBorder;

                if (oBorder != m_mStyle.end() && oBorder->second != L"none")
                    sBorder = oBorder->second;
                else
                {
                    styles_iterator oMsoBorder = m_mStyle.find(L"mso-border-alt");
                    if (oMsoBorder != m_mStyle.end() && oMsoBorder->second != L"none")
                        sBorder =  oMsoBorder->second;
                    else
                        return std::wstring();
                }

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorder);

                for (const std::wstring& sWidth : arWords)
                    if (iswdigit(sWidth[0]))
                        return sWidth;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderStyle() const
            {
                styles_iterator oBorderStyle = m_mStyle.find(L"border-style");

                if (oBorderStyle != m_mStyle.end())
                    return oBorderStyle->second;

                styles_iterator oBorder = m_mStyle.find(L"border");

                std::wstring sBorder;

                if (oBorder != m_mStyle.end() && oBorder->second != L"none")
                    sBorder = oBorder->second;
                else
                {
                    styles_iterator oMsoBorder = m_mStyle.find(L"mso-border-alt");
                    if (oMsoBorder != m_mStyle.end() && oMsoBorder->second != L"none")
                        sBorder =  oMsoBorder->second;
                    else
                        return std::wstring();
                }

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorder);

                for (std::wstring sStyle : arWords)
                {
                    std::transform(sStyle.begin(), sStyle.end(), sStyle.begin(), tolower);
                    styles_iterator oStyle = NS_CONST_VALUES::mStyles.find(sStyle);

                    if (oStyle != NS_CONST_VALUES::mStyles.end())
                        return oStyle->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderColor() const
            {
                styles_iterator oBorderColor = m_mStyle.find(L"border-color");

                if (oBorderColor != m_mStyle.end())
                    return oBorderColor->second;

                styles_iterator oBorder = m_mStyle.find(L"border");

                std::wstring sBorder;

                if (oBorder != m_mStyle.end() && oBorder->second != L"none")
                    sBorder = oBorder->second;
                else
                {
                    styles_iterator oMsoBorder = m_mStyle.find(L"mso-border-alt");
                    if (oMsoBorder != m_mStyle.end() && oMsoBorder->second != L"none")
                        sBorder =  oMsoBorder->second;
                    else
                        return std::wstring();
                }

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorder);

                for (std::wstring sColor : arWords)
                {
                    if (sColor[0] == L'#')
                    {
                        if (sColor.length() == 7)
                            return sColor.substr(1, 7);
                        else if (sColor.length() == 4)
                        {
                            std::wstring sRetColor;
                            sRetColor += sColor[1];
                            sRetColor += sColor[1];
                            sRetColor += sColor[2];
                            sRetColor += sColor[2];
                            sRetColor += sColor[3];
                            sRetColor += sColor[3];
                            return sRetColor;
                        }
                        else
                            return L"auto";
                    }

                    std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                    styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                    if (oHEX != NS_CONST_VALUES::mColors.end())
                        return oHEX->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderBottom() const
            {
                styles_iterator oBorderBottom = m_mStyle.find(L"border-bottom");

                if (oBorderBottom != m_mStyle.end())
                    return oBorderBottom->second;

                const std::wstring& sBorderBottomWidth = GetBorderBottomWidth();
                const std::wstring& sBorderBottomStyle = GetBorderBottomStyle();
                const std::wstring& sBorderBottomColor = GetBorderBottomColor();

                if (!sBorderBottomWidth.empty() && !sBorderBottomStyle.empty() && !sBorderBottomColor.empty())
                    return sBorderBottomWidth + L" " + sBorderBottomStyle + L" " + sBorderBottomColor;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderBottomWidth() const
            {
                styles_iterator oBorderBottomWidth = m_mStyle.find(L"border-bottom-width");

                if (oBorderBottomWidth != m_mStyle.end())
                    return oBorderBottomWidth->second;

                styles_iterator oBorderBottom = m_mStyle.find(L"border-bottom");

                std::wstring sBorderBottom;

                if (oBorderBottom != m_mStyle.end())
                    sBorderBottom = oBorderBottom->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderBottom);

                for (const std::wstring& sWidth : arWords)
                    if (iswdigit(sWidth[0]))
                        return sWidth;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderBottomStyle() const
            {
                styles_iterator oBorderBottomStyle = m_mStyle.find(L"border-bottom-style");

                if (oBorderBottomStyle != m_mStyle.end())
                    return oBorderBottomStyle->second;

                styles_iterator oBorderBottom = m_mStyle.find(L"border-bottom");

                std::wstring sBorderBottom;

                if (oBorderBottom != m_mStyle.end())
                    sBorderBottom = oBorderBottom->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderBottom);

                for (std::wstring sStyle : arWords)
                {
                    std::transform(sStyle.begin(), sStyle.end(), sStyle.begin(), tolower);
                    styles_iterator oStyle = NS_CONST_VALUES::mStyles.find(sStyle);

                    if (oStyle != NS_CONST_VALUES::mStyles.end())
                        return oStyle->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderBottomColor() const
            {
                styles_iterator oBorderBottomColor = m_mStyle.find(L"border-bottom-color");

                if (oBorderBottomColor != m_mStyle.end())
                    return oBorderBottomColor->second;\

                styles_iterator oBorderBottom = m_mStyle.find(L"border-bottom");

                std::wstring sBorderBottom;

                if (oBorderBottom != m_mStyle.end())
                    sBorderBottom = oBorderBottom->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderBottom);

                for (std::wstring sColor : arWords)
                {
                    if (sColor[0] == L'#')
                    {
                        if (sColor.length() == 7)
                            return sColor.substr(1, 7);
                        else if (sColor.length() == 4)
                        {
                            std::wstring sRetColor;
                            sRetColor += sColor[1];
                            sRetColor += sColor[1];
                            sRetColor += sColor[2];
                            sRetColor += sColor[2];
                            sRetColor += sColor[3];
                            sRetColor += sColor[3];
                            return sRetColor;
                        }
                        else
                            return L"auto";
                    }

                    std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                    styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                    if (oHEX != NS_CONST_VALUES::mColors.end())
                        return oHEX->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderLeft() const
            {
                styles_iterator oBorderLeft = m_mStyle.find(L"border-left");

                if (oBorderLeft != m_mStyle.end())
                    return oBorderLeft->second;

                const std::wstring& sBorderLeftWidth = GetBorderLeftWidth();
                const std::wstring& sBorderLeftStyle = GetBorderLeftStyle();
                const std::wstring& sBorderLeftColor = GetBorderLeftColor();

                if (!sBorderLeftWidth.empty() && !sBorderLeftStyle.empty() && !sBorderLeftColor.empty())
                    return sBorderLeftWidth + L" " + sBorderLeftStyle + L" " + sBorderLeftColor;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderLeftWidth() const
            {
                styles_iterator oBorderLeftWidth = m_mStyle.find(L"border-left-width");

                if (oBorderLeftWidth != m_mStyle.end())
                    return oBorderLeftWidth->second;

                styles_iterator oBorderLeft = m_mStyle.find(L"border-left");

                std::wstring sBorderLeft;

                if (oBorderLeft != m_mStyle.end())
                    sBorderLeft = oBorderLeft->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderLeft);

                for (const std::wstring& sWidth : arWords)
                    if (iswdigit(sWidth[0]))
                        return sWidth;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderLeftStyle() const
            {
                styles_iterator oBorderLeftStyle = m_mStyle.find(L"border-left-style");

                if (oBorderLeftStyle != m_mStyle.end())
                    return oBorderLeftStyle->second;

                styles_iterator oBorderLeft = m_mStyle.find(L"border-left");

                std::wstring sBorderLeft;

                if (oBorderLeft != m_mStyle.end())
                    sBorderLeft = oBorderLeft->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderLeft);

                for (std::wstring sStyle : arWords)
                {
                    styles_iterator oStyle = NS_CONST_VALUES::mStyles.find(sStyle);

                    if (oStyle != NS_CONST_VALUES::mStyles.end())
                        return oStyle->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderLeftColor() const
            {
                styles_iterator oBorderLeftColor = m_mStyle.find(L"border-left-color");

                if (oBorderLeftColor != m_mStyle.end())
                    return oBorderLeftColor->second;

                styles_iterator oBorderLeft = m_mStyle.find(L"border-left");

                std::wstring sBorderLeft;

                if (oBorderLeft != m_mStyle.end())
                    sBorderLeft = oBorderLeft->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderLeft);

                for (std::wstring sColor : arWords)
                {
                    if (sColor[0] == L'#')
                    {
                        if (sColor.length() == 7)
                            return sColor.substr(1, 7);
                        else if (sColor.length() == 4)
                        {
                            std::wstring sRetColor;
                            sRetColor += sColor[1];
                            sRetColor += sColor[1];
                            sRetColor += sColor[2];
                            sRetColor += sColor[2];
                            sRetColor += sColor[3];
                            sRetColor += sColor[3];
                            return sRetColor;
                        }
                        else
                            return L"auto";
                    }

                    const styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                    if (oHEX != NS_CONST_VALUES::mColors.end())
                        return oHEX->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderRight() const
            {
                styles_iterator oBorderRight = m_mStyle.find(L"border-right");

                if (oBorderRight != m_mStyle.end())
                    return oBorderRight->second;

                const std::wstring& sBorderRightWidth = GetBorderRightWidth();
                const std::wstring& sBorderRightStyle = GetBorderRightStyle();
                const std::wstring& sBorderRightColor = GetBorderRightColor();

                if (!sBorderRightWidth.empty() && !sBorderRightStyle.empty() && !sBorderRightColor.empty())
                    return sBorderRightWidth + L" " + sBorderRightStyle + L" " + sBorderRightColor;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderRightWidth() const
            {
                styles_iterator oBorderRightWidth = m_mStyle.find(L"border-right-width");

                if (oBorderRightWidth != m_mStyle.end())
                    return oBorderRightWidth->second;

                styles_iterator oBorderRight = m_mStyle.find(L"border-right");

                std::wstring sBorderRight;

                if (oBorderRight != m_mStyle.end())
                    sBorderRight = oBorderRight->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderRight);

                for (const std::wstring& sWidth : arWords)
                    if (iswdigit(sWidth[0]))
                        return sWidth;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderRightStyle() const
            {
                styles_iterator oBorderRightStyle = m_mStyle.find(L"border-right-style");

                if (oBorderRightStyle != m_mStyle.end())
                    return oBorderRightStyle->second;

                styles_iterator oBorderRight = m_mStyle.find(L"border-right");

                std::wstring sBorderRight;

                if (oBorderRight != m_mStyle.end())
                    sBorderRight = oBorderRight->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderRight);

                for (std::wstring sStyle : arWords)
                {
                    std::transform(sStyle.begin(), sStyle.end(), sStyle.begin(), tolower);
                    styles_iterator oStyle = NS_CONST_VALUES::mStyles.find(sStyle);

                    if (oStyle != NS_CONST_VALUES::mStyles.end())
                        return oStyle->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderRightColor() const
            {
                styles_iterator oBorderRightColor = m_mStyle.find(L"border-right-color");

                if (oBorderRightColor != m_mStyle.end())
                    return oBorderRightColor->second;

                styles_iterator oBorderRight = m_mStyle.find(L"border-right");

                std::wstring sBorderRight;

                if (oBorderRight != m_mStyle.end())
                    sBorderRight = oBorderRight->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderRight);

                for (std::wstring sColor : arWords)
                {
                    if (sColor[0] == L'#')
                    {
                        if (sColor.length() == 7)
                            return sColor.substr(1, 7);
                        else if (sColor.length() == 4)
                        {
                            std::wstring sRetColor;
                            sRetColor += sColor[1];
                            sRetColor += sColor[1];
                            sRetColor += sColor[2];
                            sRetColor += sColor[2];
                            sRetColor += sColor[3];
                            sRetColor += sColor[3];
                            return sRetColor;
                        }
                        else
                            return L"auto";
                    }

                    std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                    styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                    if (oHEX != NS_CONST_VALUES::mColors.end())
                        return oHEX->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderTop() const
            {
                styles_iterator oBorderTop = m_mStyle.find(L"border-top");

                if (oBorderTop != m_mStyle.end())
                    return oBorderTop->second;

                const std::wstring& sBorderTopWidth = GetBorderTopWidth();
                const std::wstring& sBorderTopStyle = GetBorderTopStyle();
                const std::wstring& sBorderTopColor = GetBorderTopColor();

                if (!sBorderTopWidth.empty() && !sBorderTopStyle.empty() && !sBorderTopColor.empty())
                    return sBorderTopWidth + L" " + sBorderTopStyle + L" " + sBorderTopColor;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderTopWidth() const
            {
                styles_iterator oBorderTopWidth = m_mStyle.find(L"border-top-width");

                if (oBorderTopWidth != m_mStyle.end())
                    return oBorderTopWidth->second;

                styles_iterator oBorderTop = m_mStyle.find(L"border-top");

                std::wstring sBorderTop;

                if (oBorderTop != m_mStyle.end())
                    sBorderTop = oBorderTop->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderTop);

                for (const std::wstring& sWidth : arWords)
                    if (iswdigit(sWidth[0]))
                        return sWidth;

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderTopStyle() const
            {
                styles_iterator oBorderTopStyle = m_mStyle.find(L"border-top-style");

                if (oBorderTopStyle != m_mStyle.end())
                    return oBorderTopStyle->second;

                styles_iterator oBorderTop = m_mStyle.find(L"border-top");

                std::wstring sBorderTop;

                if (oBorderTop != m_mStyle.end())
                    sBorderTop = oBorderTop->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderTop);

                for (std::wstring sStyle : arWords)
                {
                    std::transform(sStyle.begin(), sStyle.end(), sStyle.begin(), tolower);
                    styles_iterator oStyle = NS_CONST_VALUES::mStyles.find(sStyle);

                    if (oStyle != NS_CONST_VALUES::mStyles.end())
                        return oStyle->second;
                }

                return std::wstring();
            }

            std::wstring CCompiledStyle::GetBorderTopColor() const
            {
                styles_iterator oBorderTopColor = m_mStyle.find(L"border-top-color");

                if (oBorderTopColor != m_mStyle.end())
                    return oBorderTopColor->second;

                styles_iterator oBorderTop = m_mStyle.find(L"border-top");

                std::wstring sBorderTop;

                if (oBorderTop != m_mStyle.end())
                    sBorderTop = oBorderTop->second;
                else
                    return std::wstring();

                const std::vector<std::wstring>& arWords = NS_STATIC_FUNCTIONS::GetWordsW(sBorderTop);

                for (std::wstring sColor : arWords)
                {
                    if (sColor[0] == L'#')
                    {
                        if (sColor.length() == 7)
                            return sColor.substr(1, 7);
                        else if (sColor.length() == 4)
                        {
                            std::wstring sRetColor;
                            sRetColor += sColor[1];
                            sRetColor += sColor[1];
                            sRetColor += sColor[2];
                            sRetColor += sColor[2];
                            sRetColor += sColor[3];
                            sRetColor += sColor[3];
                            return sRetColor;
                        }
                        else
                            return L"auto";
                    }
                    std::transform(sColor.begin(), sColor.end(), sColor.begin(), tolower);

                    styles_iterator oHEX = NS_CONST_VALUES::mColors.find(sColor);

                    if (oHEX != NS_CONST_VALUES::mColors.end())
                        return oHEX->second;
                }

                return std::wstring();
            }
    }

