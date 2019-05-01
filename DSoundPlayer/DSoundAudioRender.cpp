#include "stdafx.h"
#include "DSoundAudioRender.h"

#define BUF_NUM 4
#define AudioSampleRate 44100
DSoundAudioRender::DSoundAudioRender()
    : m_bufferIndexL(-1)
    , m_bufferNext(-1) {
}

bool DSoundAudioRender::init() {
    HRESULT hr = DirectSoundCreate8(&DSDEVID_DefaultPlayback, &m_pDevice, NULL);
    if (hr != S_OK) {
        printf("[error] init DirectSoundCreate8\n");
        return false;
    }

    m_pDevice->Initialize(NULL);
    HWND hWnd = ::GetDesktopWindow();
    hr = m_pDevice->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
    if (hr != S_OK) {
        m_pDevice.Release();
        printf("[error] init SetCooperativeLevel\n");
        return false;
    }

    for (size_t i = 0; i < 2 * BUF_NUM + 1; i++) {
        m_hNotifyEv.push_back(::CreateEvent(NULL, FALSE, FALSE, NULL));
    }

    return true;
}

int DSoundAudioRender::configuration() {
    DSBUFFERDESC&  cDSBufferDesc = m_cDSBufferDesc;
    memset(&cDSBufferDesc, 0, sizeof(cDSBufferDesc));

    cDSBufferDesc.dwBufferBytes = /*meta.getAudioBlockAlign()*/192000*BUF_NUM;
    cDSBufferDesc.dwFlags =
        DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
        DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPOSITIONNOTIFY;
    cDSBufferDesc.dwReserved = NULL;
    cDSBufferDesc.dwSize = sizeof(cDSBufferDesc);

    WAVEFORMATEX& waveFmt = m_waveFmt;
    waveFmt.cbSize = sizeof(WAVEFORMATEX);
    waveFmt.wFormatTag = WAVE_FORMAT_PCM;
    waveFmt.nChannels = 2;// meta.getAudioChanel();
    waveFmt.nSamplesPerSec = AudioSampleRate;// meta.getAudioSampleRate();
    waveFmt.wBitsPerSample = 16;// meta.getAudioBpp();
    waveFmt.nAvgBytesPerSec = (__int64)waveFmt.nSamplesPerSec*
        waveFmt.nChannels *waveFmt.wBitsPerSample >> 3;
    waveFmt.nBlockAlign = waveFmt.nChannels*waveFmt.wBitsPerSample >> 3;

    cDSBufferDesc.lpwfxFormat = &waveFmt;

/*#if 1
    m_blockDuration = (long long)cDSBufferDesc.dwBufferBytes * 1000000 / (waveFmt.nAvgBytesPerSec*BUF_NUM);

    //cDSBufferDesc.dwBufferBytes *= 2;
    //m_fillCountPer = 2;
    m_fillCountPer = 1;
#else	
    if (m_blockDuration < 18750) {
        cDSBufferDesc.dwBufferBytes *= 4;
        m_fillCountPer = 4;
    }
    else if (m_blockDuration < 25000) {
        cDSBufferDesc.dwBufferBytes *= 3;
        m_fillCountPer = 3;
    }
    else if (m_blockDuration < 50000) {
        cDSBufferDesc.dwBufferBytes *= 2;
        m_fillCountPer = 2;
    }
    else {
        m_fillCountPer = 1;
    }
    printf("[DSAudioRender] blockDuration=%d us\n", m_blockDuration);
#endif*/
    
    HRESULT hr = m_pDevice->CreateSoundBuffer(&cDSBufferDesc, &m_pAudioBuffer, NULL);

    if (hr != S_OK) {
        printf("[DSAudioRender] Failed CreateSoundBuffer\n");
        return -1;
    }

    hr = m_pAudioBuffer->QueryInterface(IID_IDirectSoundNotify, (void**)&m_pNotify);
    if (hr != S_OK) {
        printf("[DSAudioRender] Failed QueryInterface IDirectSoundNotify\n");
        return -1;
    }

    DSBPOSITIONNOTIFY PositionNotify[BUF_NUM * 2 + 1];
    int eventCount = sizeof(PositionNotify) / sizeof(PositionNotify[0]);
    for (int i = 0; i < eventCount; i++) {
        PositionNotify[i].dwOffset = cDSBufferDesc.dwBufferBytes*i / (BUF_NUM * 2);//DSBPN_OFFSETSTOP;
        PositionNotify[i].hEventNotify = m_hNotifyEv[i];
        if (i == eventCount - 1) {
            PositionNotify[i].dwOffset = DSBPN_OFFSETSTOP;
        }
    }
    hr = m_pNotify->SetNotificationPositions(eventCount, PositionNotify);
    if (hr != S_OK) {
        printf("[DSAudioRender] Failed SetNotificationPositions\n");
        return -1;
    }

    /*TsMediaCorePtr mc(GetMediaCore());
    if (!mc)
        return -1;
    this->setVolumeImpl(mc->GetAudioRenderVolume());

    m_notifyCbkMetaData = meta.clone();*/
    // create mute buffer
    printf("[DSAudioRender] ch=%d, sr=%d, BufferAlign=%d \n",
        cDSBufferDesc.lpwfxFormat->nChannels,
        cDSBufferDesc.lpwfxFormat->nSamplesPerSec,
        cDSBufferDesc.dwBufferBytes / BUF_NUM);
    waveFmt.nSamplesPerSec = AudioSampleRate;// meta.getAudioSampleRate();
    return 0;
}

void DSoundAudioRender::play() {

}

void DSoundAudioRender::loop() {
    DWORD result;
    while (!m_stop) {
        result = ::WaitForMultipleObjects(m_hNotifyEv.size(), &m_hNotifyEv[0], FALSE, 500);
        if (WAIT_OBJECT_0 <= result && result < WAIT_OBJECT_0 + BUF_NUM * 2) {
            renderFrames();
        }
        else if (result == WAIT_OBJECT_0 + 2 * BUF_NUM) {
            printf("[DSAudioRender] callback Stop\n");
            do {
                FlushBuffers();
            } while (0);

        }
        else if (result == WAIT_TIMEOUT) {
            do {
                FlushBuffers();
            } while (0);
            do {
                std::lock_guard<std::mutex> lk(m_dsLock);
                DWORD status;
                HRESULT hr = m_pAudioBuffer->GetStatus(&status);
                printf("[DSoundAudioRender] hr=%x, status = %x\n", hr, status);
            } while (0);
        }
        else {
            break;
        }
    }
}

void DSoundAudioRender::renderFrames() {
    DWORD           dwCurR, dwCurW;
    int bufferSize = m_cDSBufferDesc.dwBufferBytes / BUF_NUM;
    m_pAudioBuffer->GetCurrentPosition(&dwCurR, &dwCurW);
    int writeIndex = dwCurW / bufferSize;
    int bufferIndex = (writeIndex + 2) % BUF_NUM;

    //TsLogI("[DSAudioRender] render event %d, %d, dwCurR=%04d, dwCurW=%04d\n", m_bufferIndexL, bufferIndex, dwCurR, dwCurW);
    if ((m_bufferIndexL != -1) && (bufferIndex != m_bufferNext)) {
        if (m_bufferIndexL == bufferIndex)
            return;
        int skip = (bufferIndex - m_bufferNext + BUF_NUM) % BUF_NUM;
        if (skip == 1) {
            bufferIndex = m_bufferNext;
        }
        else {
            printf("[DSAudioRender] render event %d, %d, dwCurR=%04d, dwCurW=%04d\n", m_bufferIndexL, bufferIndex, dwCurR, dwCurW);
            printf("[DSAudioRender] lost render event %d, %d\n", m_bufferIndexL, bufferIndex);
        }
    }

    m_bufferIndexL = bufferIndex;

#if    DS_DEBUG
    long long renderTime = m_renderTime * 1000000 / m_cDSBufferDesc.lpwfxFormat->nAvgBytesPerSec;
    int errortime = m_acrCalcer.AddFrame(renderTime);
    m_renderTime += m_cDSBufferDesc.dwBufferBytes / BUF_NUM;

    this->m_frameId++;
    int avg = (PlatformGetUTime() - m_acrCalcer.getStartLocalTime()) / this->m_frameId;

    ::TsLogI("[DSAudioRender] MonitorProc rd=%s, errortime=%s, avg=%s\n",
        TsDebugLog()(renderTime), TsDebugLog()(errortime), TsDebugLog()(avg));
#endif
    TsPacketBuffer packets[4];
    int count = m_fillCountPer;
    CLDTS::TsMetaData mixedAudioMeta;
    bool fromRemote = false;
    int connectedPlayerCount = 0;
    GetMixedPacketBuffer(packets[0], mixedAudioMeta, fromRemote, connectedPlayerCount);
#if  DS_DEBUG
    int errortime = m_acrCalcer.AddFrame(packets[0].getTimeStamp());
    m_renderTime += m_cDSBufferDesc.dwBufferBytes / BUF_NUM;
    ::TsLogI("[DSAudioRender] MonitorProc rd=%s, errortime=%s\n",
        TsDebugLog()(renderTime), TsDebugLog()(errortime));
#endif

    for (int i = 0; i < count; i++) {
        TsPacketBuffer packet2AEC = packets[i];
        // TODO: should set vad_activity_ for AEC in webrtc
        CLDTS::TsAudioRenderDataDispacher::instace().Dispatch(packet2AEC, mixedAudioMeta,
            fromRemote, connectedPlayerCount);
    }
    do {
        std::lock_guard<std::mutex> lk(m_dsLock);
        FillAudioData(packets, count, bufferIndex);

        DWORD           dwCurR, dwCurW;
        m_pAudioBuffer->GetCurrentPosition(&dwCurR, &dwCurW);
    } while (0);
    bufferIndex++;

    m_bufferNext = bufferIndex%BUF_NUM;
}

DSoundAudioRender::~DSoundAudioRender()
{
}
