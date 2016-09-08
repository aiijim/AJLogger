//
//  AJLogFilterChain.cpp
//  AJLogger
//
//  Created by aiijim on 16/8/15.
//  Copyright © 2016年 aiijim. All rights reserved.
//

#include "AJLoggerPlus.h"

namespace AJLoggerPlus
{
    std::vector<AJLogFilter*> AJLogFilterChain::m_filterChain;
    
    void AJLogFilterChain::addFilter(AJLogFilter* filter)
    {
        for (std::vector<AJLogFilter*>::iterator iter = AJLogFilterChain::m_filterChain.begin(); iter != AJLogFilterChain::m_filterChain.end(); iter++)
        {
            if (*iter == filter)
            {
                return;
            }
        }

        AJLogFilterChain::m_filterChain.push_back(filter);
    }
    
    void AJLogFilterChain::removeLogFilter(AJLogFilter* filter)
    {
        for (std::vector<AJLogFilter*>::iterator iter = AJLogFilterChain::m_filterChain.begin(); iter != AJLogFilterChain::m_filterChain.end(); iter++)
        {
            if (*iter == filter)
            {
                AJLogFilterChain::m_filterChain.erase(iter);
            }
        }
    }
    
    bool AJLogFilterChain::doFilter(AJLogMessage* ptMsg,AJLogFilterChain* filterChain)
    {
        if (m_index >= AJLogFilterChain::m_filterChain.size())
        {
            return false;
        }
        
        AJLogFilter* filter = AJLogFilterChain::m_filterChain.at(m_index);
        m_index++;
        return filter->doFilter(ptMsg, filterChain);
    }
}
