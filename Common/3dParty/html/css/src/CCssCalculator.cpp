#include "CCssCalculator.h"
#include "CCssCalculator_Private.h"


namespace NSCSS
{
    CCssCalculator::CCssCalculator()
    {
        m_pInternal = new CCssCalculator_Private();
    }

    CCssCalculator::~CCssCalculator()
    {
        delete m_pInternal;
    }

    CCompiledStyle CCssCalculator::GetCompiledStyle(const std::vector<CNode> &arSelectors, const UnitMeasure& unitMeasure) const
    {
        return  m_pInternal->GetCompiledStyle(arSelectors, unitMeasure);
    }

    /*
    void CCssCalculator::AddStyle(const std::vector<std::string>& sSelectors, const std::string &sStyle)
    {
        m_pInternal->AddStyle(sSelectors, sStyle);
    }
    */

    void CCssCalculator::AddStyles(const std::string &sStyle)
    {
        m_pInternal->AddStyles(sStyle);
    }

    void CCssCalculator::AddStyles(const std::wstring &sStyle)
    {
        m_pInternal->AddStyles(sStyle);
    }

    void CCssCalculator::AddStylesFromFile(const std::wstring &sFileName)
    {
        m_pInternal->AddStylesFromFile(sFileName);
    }

    void CCssCalculator::SetUnitMeasure(const UnitMeasure& nType)
    {
        m_pInternal->SetUnitMeasure(nType);
    }

    void CCssCalculator::SetDpi(const unsigned short int& nValue)
    {
        m_pInternal->SetDpi(nValue);
    }

    UnitMeasure CCssCalculator::GetUnitMeasure() const
    {
        return m_pInternal->GetUnitMeasure();
    }

    std::wstring CCssCalculator::GetEncoding() const
    {
        return m_pInternal->GetEncoding();
    }

    unsigned short int CCssCalculator::GetDpi() const
    {
        return m_pInternal->GetDpi();
    }

    void CCssCalculator::Clear()
    {
        m_pInternal->Clear();
    }
}
