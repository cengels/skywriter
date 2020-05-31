#include "ErrorManager.h"

namespace {
    ErrorManager* m_instance;
}

ErrorManager::ErrorManager(QObject *parent) : QObject(parent) { }

ErrorManager* ErrorManager::instance()
{
    if (!m_instance) {
        m_instance = new ErrorManager();
    }

    return m_instance;
}
