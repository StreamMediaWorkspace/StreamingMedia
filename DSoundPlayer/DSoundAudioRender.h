#pragma once
#include <dsound.h>
#include <atlcomcli.h>
#include <vector>
#include <mutex>
class DSoundAudioRender
{
public:
    DSoundAudioRender();
    ~DSoundAudioRender();

    bool init();
    void play();

private:
    void loop();

    int configuration();
    void renderFrames();

private:
    ::CComPtr<IDirectSound8>        m_pDevice;
    ::CComPtr<IDirectSoundBuffer>   m_pAudioBuffer;
    ::CComPtr<IDirectSoundNotify>  m_pNotify;

    std::vector<HANDLE>  m_hNotifyEv;
    std::mutex 			m_dsLock;
    DSBUFFERDESC  m_cDSBufferDesc;
    WAVEFORMATEX m_waveFmt;

    int m_bufferIndexL;
    int m_bufferNext;

    bool m_stop = false;
};

