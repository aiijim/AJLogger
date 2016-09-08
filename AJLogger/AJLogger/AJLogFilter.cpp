//
//  AJLogFilter.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/15.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerPlus.h"

namespace AJLoggerPlus
{
    bool AJLogFilter::doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain)
    {
        if (m_callback == NULL)
        {
            return false;
        }
        
        return m_callback(ptMsg) ? true : filterChain->doFilter(ptMsg, filterChain);
    };
    
    
    AJLogModuleFilter::AJLogModuleFilter()
    {
        m_callback = NULL;
        m_excludeMode = true;
    }
    
    bool AJLogModuleFilter::doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain)
    {
        std::vector<std::string> filters = m_mdlFilter;

        bool isFind = false;
        for (std::vector<std::string>::iterator iter = filters.begin(); iter != filters.end(); iter++)
        {
            if (*iter == ptMsg->m_moduleName)
            {
                isFind = true;
                break;
            }
        }

        return !(m_excludeMode ^ isFind);
    }
    
    void AJLogModuleFilter::setModuleFilter(std::vector<std::string> &mdls)
    {
        m_mdlFilter = mdls;
    }
    
    void AJLogModuleFilter::setModuleFilter(const char* mdls[], int len)
    {
        m_mdlFilter.clear();
        for (int i = 0; i < len; i++)
        {
            m_mdlFilter.push_back(mdls[i]);
        }
    }
    
    void AJLogModuleFilter::setModuleFilter(int count, ...)
    {
        m_mdlFilter.clear();
        va_list vp;
        va_start(vp, count);
        for (int i = 0; i < count; i++)
        {
            const char* temp = va_arg(vp, const char *);
            m_mdlFilter.push_back(temp);
        }
        va_end(vp);
    }
}
