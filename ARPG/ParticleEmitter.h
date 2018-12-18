/*-------------------------------------------------------
Special Effects Game Programming with DirectX
By MASON MCCUSKEY 
-------------------------------------------------------*/
#ifndef PARTICLEEMITTER_H
#define PARTICLEEMITTER_H

#include <string>
#include <map>
#include "Particle.h"

#define NUMPARTICLES 5000

typedef struct 
{
    D3DXVECTOR3 position;
    float       pointsize;
    D3DCOLOR    color;
} VERTEX_PARTICLE;

#define D3DFVF_PARTICLE (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_PSIZE)

template <class T> 
class CMinMax
{
public: 
	CMinMax() { m_Min = m_Max = 0; }
    CMinMax(T tMin, T tMax) { m_Min = tMin; m_Max = tMax; }
    ~CMinMax() { }

	T m_Min;
    T m_Max;

    T GetRandomNumInRange(void) { return (T)(RandomNumber((int)m_Min, (int)m_Max)); }
};

typedef class CMinMax<int> CMinMaxInt;
typedef class CMinMax<float> CMinMaxFloat;

typedef struct 
{
	char name[25];
	int  mode;
	char help[256];
} BLENDINGMODE;

class CParticleEmitter
{
public:
	static const int NUMBLENDINGMODES;
    static const BLENDINGMODE m_BlendingModes[15];
    std::string m_strLastError;
  
	CParticleEmitter();
	virtual ~CParticleEmitter();

	virtual void Update(float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDev);
	virtual void InvalidateDeviceObjects();  
	
	// attributes
	D3DXVECTOR3 GetGravity(void) const         { return m_vGravity; }
	void SetGravity(const D3DXVECTOR3 &data)   { m_vGravity = data; }
	D3DXVECTOR3 GetPos(void) const             { return m_vPos; }
	void SetPos(const D3DXVECTOR3 &data)       { m_vPos = data; }

	CMinMax<float> GetEmitRate(void) const  { return m_fEmitRate; }
	void SetEmitRate(const CMinMax<float> data)      { m_fEmitRate = data; }

	CMinMax<float> GetLifeTime(void) const {return m_fLifetime;}
	void SetLifeTime(const CMinMax<float> data) {m_fLifetime=data;}

	CMinMax<float> GetSize(void) const {return m_fSize;}
	void SetSize(const CMinMax<float> data) {m_fSize=data;}

	D3DXCOLOR GetStartColor1(void) const            { return m_StartColor1; }
	void SetStartColor1(const D3DXCOLOR &data)      { m_StartColor1 = data; }

	D3DXCOLOR GetStartColor2(void) const            { return m_StartColor2;}
	void SetStartColor2(const D3DXCOLOR &data)      { m_StartColor2 = data; }

	D3DXCOLOR GetEndColor1(void) const {return m_EndColor1;}
	void SetEndColor1(const D3DXCOLOR &data) {m_EndColor1=data;}

	D3DXCOLOR GetEndColor2(void) const {return m_EndColor2;}
	void SetEndColor2(const D3DXCOLOR &data) {m_EndColor2=data;}
	
	D3DXVECTOR3 GetSpawnDir1(void) const       { return m_vSpawnDir1;}
	void SetSpawnDir1(const D3DXVECTOR3 &data) { m_vSpawnDir1 = data; }

	D3DXVECTOR3 GetSpawnDir2(void) const       { return m_vSpawnDir2;}
	void SetSpawnDir2(const D3DXVECTOR3 &data) { m_vSpawnDir2 = data; }

	D3DXVECTOR3 GetEmitRadius(void) const { return(m_vEmitRadius); }
    void SetEmitRadius(const D3DXVECTOR3 &data) { m_vEmitRadius = data; }

    std::string GetTextureFilename(void) const { return(m_strTexFilename); }
    LPDIRECT3DTEXTURE9 GetTexture(void) const { return(m_texParticle); }
    void SetTexture(const char *strTexFilename);

	int GetVBSize(void) const                  { return m_iVBSize;}
	void SetVBSize(const int data)             { m_iVBSize=data;}

	int GetMaxParticles(void) const { return(m_iMaxParticles); }
    void SetMaxParticles(const int data) { m_iMaxParticles = data; }
  
    int GetSrcBlendMode(void) const { return(m_iSrcBlendMode); }
    void SetSrcBlendMode(const int data) { m_iSrcBlendMode = data; }

    int GetDestBlendMode(void) const { return(m_iDestBlendMode); }
    void SetDestBlendMode(const int data) { m_iDestBlendMode = data; }

    int GetNumActiveParticles(void) { return(m_Particles.GetNumUsedElements()); }

    void Reset(void) { m_Particles.DeleteAll(); }

	void SetShowGround(const bool data) {m_bShowGround=data;}
	bool GetShowGround(void) const {return m_bShowGround;}

    bool Save(const char *strFilename);
    bool Load(const char *strFilename);
  
private:
	void PluckFirstField(char *str, char *dest, int maxlen,const char *delim);

    void GetRightSideFromValueMap(std::map<std::string, std::string> &valuemap,               
		const char *strName, char *dest, int destsize);
  
	void ReadProperty(std::map<std::string, std::string> &valuemap, const char *strName, D3DXCOLOR &value);
    void ReadProperty(std::map<std::string, std::string> &valuemap, const char *strName, D3DXVECTOR3 &value);
    void ReadProperty(std::map<std::string, std::string> &valuemap, const char *strName, CMinMax<float> &value);
    void ReadProperty(std::map<std::string, std::string> &valuemap, const char *strName, int &value);
    void ReadProperty(std::map<std::string, std::string> &valuemap, const char *strName, std::string &value);
    void ReadBlendModeProperty(std::map<std::string, std::string> &valuemap, const char *strName, int &value);

	// particle system attributes
	D3DXVECTOR3 m_vGravity;
	D3DXVECTOR3 m_vPos; // position of emitter
	// these vars determine starting values for each particle
    CMinMax<float> m_fEmitRate; // in particles/sec
    CMinMax<float> m_fLifetime; // in particles/sec
    CMinMax<float> m_fSize; // in particles/sec
	// these vars determine starting values for each particle
	D3DXCOLOR m_StartColor1;
	D3DXCOLOR m_StartColor2;
	D3DXCOLOR m_EndColor1;
	D3DXCOLOR m_EndColor2;
	
	D3DXVECTOR3 m_vSpawnDir1;
	D3DXVECTOR3 m_vSpawnDir2;

	D3DXVECTOR3 m_vEmitRadius;
	
	bool m_bShowGround;

	// other stuff
    int m_iDestBlendMode;
    int m_iSrcBlendMode;
  
    int m_iMaxParticles;

	std::string m_strTexFilename;
	float m_fNumNewPartsExcess;
	int m_iVBSize; // size of the vertex buffer (can and should be less than NUMPARTICLES)
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9 m_vbParticles;
	LPDIRECT3DTEXTURE9 m_texParticle;

	CRecyclingArray<CParticle, NUMPARTICLES> m_Particles;
};
#endif
