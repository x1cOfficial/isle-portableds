#include "mxeventmanager.h"

#include "mxcriticalsection.h"
#include "mxmisc.h"
#include "mxticklemanager.h"
#include "mxticklethread.h"

// FUNCTION: LEGO1 0x100c0360
MxEventManager::MxEventManager()
{
	Init();
}

// FUNCTION: LEGO1 0x100c03f0
MxEventManager::~MxEventManager()
{
	Destroy(TRUE);
}

// FUNCTION: LEGO1 0x100c0450
void MxEventManager::Init()
{
	// This is intentionally left blank
}

// FUNCTION: LEGO1 0x100c0460
void MxEventManager::Destroy(MxBool p_fromDestructor)
{
	if (m_thread != NULL) {
		m_thread->Terminate();
		delete m_thread;
	}
	else {
		TickleManager()->UnregisterClient(this);
	}

	if (!p_fromDestructor) {
		MxMediaManager::Destroy();
	}
}

// FUNCTION: LEGO1 0x100c04a0
MxResult MxEventManager::Create(MxU32 p_frequencyMS, MxBool p_createThread)
{
	MxResult status = FAILURE;
	MxBool locked = FALSE;

	MxResult result = MxMediaManager::Create();
	if (result == SUCCESS) {
		if (p_createThread) {
			ENTER(this->m_criticalSection);
			locked = TRUE;
			this->m_thread = new MxTickleThread(this, p_frequencyMS);

			if (!this->m_thread || this->m_thread->Start(0, 0) != SUCCESS) {
				goto done;
			}
		}
		else {
			TickleManager()->RegisterClient(this, p_frequencyMS);
		}

		status = SUCCESS;
	}

done:
	if (status != SUCCESS) {
		Destroy();
	}

	if (locked) {
		this->m_criticalSection.Leave();
	}

	return status;
}

// FUNCTION: LEGO1 0x100c0590
void MxEventManager::Destroy()
{
	Destroy(FALSE);
}
