#include <fstream>
#include "ParticleEmitter.h"

using std::ofstream;
using std::fstream;
using std::endl;
using std::map;
using std::string;
#define SAFE_RELEASE(x) { if(x) { (x)->Release(); (x)=NULL; } }

const int CParticleEmitter::NUMBLENDINGMODES = 15;
const BLENDINGMODE CParticleEmitter::m_BlendingModes[15] =
{
	{ "D3DBLEND_ZERO", D3DBLEND_ZERO ,"Blend factor is (0, 0, 0, 0)."},
    { "D3DBLEND_ONE", D3DBLEND_ONE ,"Blend factor is (1, 1, 1, 1)."},
    { "D3DBLEND_SRCCOLOR", D3DBLEND_SRCCOLOR,"Blend factor is (Rs, Gs, Bs, As)." },
    { "D3DBLEND_INVSRCCOLOR", D3DBLEND_INVSRCCOLOR,"Blend factor is (1 - Rs, 1 - Gs, 1 - Bs, 1 - As)." },
    { "D3DBLEND_SRCALPHA", D3DBLEND_SRCALPHA,"Blend factor is (As, As, As, As)."},
    { "D3DBLEND_INVSRCALPHA", D3DBLEND_INVSRCALPHA,"Blend factor is ( 1 - As, 1 - As, 1 - As, 1 - As)."},
    { "D3DBLEND_DESTALPHA", D3DBLEND_DESTALPHA,"Blend factor is (Ad Ad Ad Ad). "},
    { "D3DBLEND_INVDESTALPHA", D3DBLEND_INVDESTALPHA,"Blend factor is (1 - Ad 1 - Ad 1 - Ad 1 - Ad)."},
    { "D3DBLEND_DESTCOLOR", D3DBLEND_DESTCOLOR,"Blend factor is (Rd, Gd, Bd, Ad)."},
    { "D3DBLEND_INVDESTCOLOR", D3DBLEND_INVDESTCOLOR,"Blend factor is (1 - Rd, 1 - Gd, 1 - Bd, 1 - Ad)." },
    { "D3DBLEND_SRCALPHASAT", D3DBLEND_SRCALPHASAT,"Blend factor is (f, f, f, 1); where f = min(As, 1 - Ad)."},
	{ "D3DBLEND_BLENDFACTOR ",D3DBLEND_BLENDFACTOR,"Constant color blending factor used by the frame-buffer blender. This blend mode is supported only if D3DPBLENDCAPS_BLENDFACTOR is set in the SrcBlendCaps or DestBlendCaps members of D3DCAPS9."},
	{ "D3DBLEND_INVBLENDFACTOR",D3DBLEND_INVBLENDFACTOR ,"Inverted constant color-blending factor used by the frame-buffer blender. This blend mode is supported only if the D3DPBLENDCAPS_BLENDFACTOR bit is set in the SrcBlendCaps or DestBlendCaps members of D3DCAPS9."},
	{ "D3DBLEND_SRCCOLOR2",D3DBLEND_SRCCOLOR2,"Blend factor is (PSOutColor[1]r, PSOutColor[1]g, PSOutColor[1]b, not used)."},
	{ "D3DBLEND_INVSRCCOLOR2",D3DBLEND_INVSRCCOLOR2,"Blend factor is (1 - PSOutColor[1]r, 1 - PSOutColor[1]g, 1 - PSOutColor[1]b, not used))."}
};

CParticleEmitter::CParticleEmitter()
{
	// default particle system configuration - boring!
	SetGravity(D3DXVECTOR3(0.0f,0.0f,0.0f));
	SetPos(D3DXVECTOR3(0.0f,0.0f,0.0f));
	
	SetEmitRate(CMinMax<float>(1.0f, 10.0f));
    SetSize(CMinMax<float>(20.0f, 100.0f));

	SetStartColor1(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
    SetStartColor2(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
    SetEndColor1(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));
    SetEndColor2(D3DXCOLOR(1.0f,1.0f,1.0f,1.0f));

	SetSpawnDir1(D3DXVECTOR3(-1.0f, -1.0f, -1.0f));
	SetSpawnDir2(D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	SetEmitRadius(D3DXVECTOR3(1.0f,1.0f,1.0f));

	SetSrcBlendMode(D3DBLEND_ONE);
    SetDestBlendMode(D3DBLEND_ONE);

	m_bShowGround=false;
 
	// initialize misc. other things
	m_pd3dDevice = NULL;
	SetVBSize(NUMPARTICLES / 10);
	m_fNumNewPartsExcess = 0.0f;
	m_texParticle=NULL;
	m_iMaxParticles=NUMPARTICLES;
}

CParticleEmitter::~CParticleEmitter()
{
}

HRESULT CParticleEmitter::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDev)
{
	HRESULT hr;
	m_pd3dDevice = pDev;
	m_Particles.DeleteAll();

	// load up the particle system texture
	if (FAILED(hr = D3DXCreateTextureFromFile(m_pd3dDevice,m_strTexFilename.c_str(), &m_texParticle))) 
		return(hr);
  
	// create vertex buffer
	//由于使用D3DUSAGE_POINTS，即DX内置的粒子渲染功能，故顶点格式中取消纹理坐标，
	//点位置（即对应渲染的正方行中心点）
	if(FAILED(hr = m_pd3dDevice->CreateVertexBuffer(m_iVBSize * sizeof(VERTEX_PARTICLE), 
		D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS,//关键 D3DUSAGE_POINTS
		D3DFVF_PARTICLE, D3DPOOL_DEFAULT, &m_vbParticles,NULL))) 
		return(hr);

	return S_OK;	
}

void CParticleEmitter::InvalidateDeviceObjects()
{
	SAFE_RELEASE(m_texParticle);
	SAFE_RELEASE(m_vbParticles);
}

void CParticleEmitter::Update(float fTimeDelta)//float fElapsedTime, float fTimeDelta)
{
	// update existing particles
	{
		for (int q=0; q < m_Particles.GetTotalElements(); q++) 
		{   
			if (m_Particles.IsAlive(q))
			{   
				CParticle &part = m_Particles.GetAt(q);
				if (!part.Update(fTimeDelta)) 
				{          
					m_Particles.Delete(&part);
				}//is dead 清空这个位置
				else
				{
					part.m_vDir+=fTimeDelta*m_vGravity;
				}
			} // is alive
		} // next particle
	}
  
	// create new particles
	{
		// determine the number of particles we need to create
		//此帧发射粒子速度，随机值（介于m_fMinEmitRate和m_fMaxEmitRate之间）
		float fEmitRateThisFrame = m_fEmitRate.GetRandomNumInRange();
		//此帧发射的粒子总数
		int   iNumNewParts       =(int)( fEmitRateThisFrame * fTimeDelta);
		//累计剩余粒子总数（累计每次发射数的小数部分）
		m_fNumNewPartsExcess    += (float)(fEmitRateThisFrame * fTimeDelta)-iNumNewParts;
		//超过1，此帧发射粒子总数加1，m_fNumNewPartsExcess值要减1
		if (m_fNumNewPartsExcess > 1.0f)
		{
			iNumNewParts += (int)m_fNumNewPartsExcess;
			m_fNumNewPartsExcess -= (int)m_fNumNewPartsExcess;
		}
		//制造iNumNewParts个粒子
		for (int q=0; q < iNumNewParts&&m_Particles.GetNumUsedElements()<m_iMaxParticles; q++) 
		{    
			try
			{
				CParticle *part = m_Particles.New();
				part->m_fLifetime=m_fLifetime.GetRandomNumInRange();//生存时间
				part->m_fSize=m_fSize.GetRandomNumInRange();//大小
				// determine a random vector between dir1 and dir2       
				float fRandX =(float)RandomNumber((int)m_vSpawnDir1.x, (int)m_vSpawnDir2.x);
				float fRandY =(float)RandomNumber((int)m_vSpawnDir1.y, (int)m_vSpawnDir2.y);
				float fRandZ =(float)RandomNumber((int)m_vSpawnDir1.z, (int)m_vSpawnDir2.z); 
				part->m_vDir = D3DXVECTOR3(fRandX, fRandY, fRandZ);//初速度
				part->m_vPos = m_vPos;//初始位置
				fRandX =(float) RandomNumber(-(int)m_vEmitRadius.x,(int)m_vEmitRadius.x);
				fRandY =(float) RandomNumber(-(int)m_vEmitRadius.y,(int)m_vEmitRadius.y);
                fRandZ =(float) RandomNumber(-(int)m_vEmitRadius.z,(int)m_vEmitRadius.z);       
				part->m_vPos += D3DXVECTOR3(fRandX, fRandY, fRandZ);//运动范围

				float fRandR = (float)RandomNumber((int)m_StartColor1.r, (int)m_StartColor2.r);
				float fRandG = (float)RandomNumber((int)m_StartColor1.g, (int)m_StartColor2.g);
				float fRandB = (float)RandomNumber((int)m_StartColor1.b, (int)m_StartColor2.b);
				float fRandA = (float)RandomNumber((int)m_StartColor1.a, (int)m_StartColor2.a);
				D3DXCOLOR startColor=D3DXCOLOR(fRandA,fRandR,fRandG,fRandB);//开始颜色

				fRandR = (float)RandomNumber((int)m_EndColor1.r, (int)m_EndColor2.r);
                fRandG = (float)RandomNumber((int)m_EndColor1.g, (int)m_EndColor2.g);
                fRandB = (float)RandomNumber((int)m_EndColor1.b, (int)m_EndColor2.b);
                fRandA = (float)RandomNumber((int)m_EndColor1.a, (int)m_EndColor2.a);
                D3DXCOLOR endColor(fRandR, fRandG, fRandB, fRandA);//结束颜色

				part->m_Color = startColor;//初始颜色
				part->m_ColorStep=(endColor-startColor)/part->m_fLifetime;//单位时间颜色变化量，粒子颜色是渐变的
			} 
			catch(...) 
			{ 
				q = iNumNewParts;
			}
    
		}
  
	}

}

HRESULT CParticleEmitter::Render()
{
	HRESULT hr;

	// Set the render states for using point sprites
	m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(0.08f) );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(m_fSize.m_Min) );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(m_fSize.m_Max));
	m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,GetSrcBlendMode());
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,GetDestBlendMode());

	// Set up the vertex buffer to be rendered
	m_pd3dDevice->SetStreamSource( 0, m_vbParticles, 0,sizeof(VERTEX_PARTICLE) ); 
	m_pd3dDevice->SetFVF( D3DFVF_PARTICLE );
	m_pd3dDevice->SetTexture(0, m_texParticle);
  
	int iCurStart=0;
	VERTEX_PARTICLE *pVertices;
	DWORD dwNumParticlesToRender = 0;
	if(FAILED(hr = m_vbParticles->Lock(iCurStart*sizeof(VERTEX_PARTICLE), m_iVBSize * sizeof(VERTEX_PARTICLE),    
		(void **) &pVertices, iCurStart?D3DLOCK_NOOVERWRITE:D3DLOCK_DISCARD)))
		return hr;

	// Render each particle
	for (int q=0; q < NUMPARTICLES; q++) 
	{
		// Render each particle a bunch of times to get a blurring effect
		if (m_Particles.IsAlive(q)) 
		{
			CParticle &part = m_Particles.GetAt(q);
      
			//顶点数据赋值
			pVertices->position = part.m_vPos;
			pVertices->pointsize = 1.0f;
			pVertices->color = (DWORD)part.m_Color;
    
			pVertices++;

			if( ++dwNumParticlesToRender == m_iVBSize )
			{
				// Done filling this chunk of the vertex buffer.  Lets unlock and
				// draw this portion so we can begin filling the next chunk.
				m_vbParticles->Unlock();
				if(FAILED(hr = m_pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, iCurStart, dwNumParticlesToRender)))
					return hr;

				//D3DLOCK_DISCARD 丢弃锁定区域的所有内容，因为将数据传至显卡
				iCurStart=0;
				if(FAILED(hr = m_vbParticles->Lock(0, m_iVBSize * sizeof(VERTEX_PARTICLE),
					(void **) &pVertices, D3DLOCK_DISCARD)))          
					return hr;
				dwNumParticlesToRender = 0;    
			}
		}
	}
	// Unlock the vertex buffer  
	m_vbParticles->Unlock();

	// Render any remaining particles
	if( dwNumParticlesToRender )
	{
		if(FAILED(hr = m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, iCurStart, dwNumParticlesToRender )))
			return hr;
	}

	// Reset render states
	m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );

	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE,      TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,  FALSE );	
//	m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );	
//	m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	return S_OK;
}
void CParticleEmitter::SetTexture(const char *strTexFilename)
{ 
	m_strTexFilename = strTexFilename;
    if (m_pd3dDevice) 
	{
		SAFE_RELEASE(m_texParticle);
        if (FAILED(D3DXCreateTextureFromFile(m_pd3dDevice,m_strTexFilename.c_str(), &m_texParticle))) 
		{
			m_texParticle = NULL;
		}
	}
}

void CParticleEmitter::GetRightSideFromValueMap(map<string, string> &valuemap, 
                                   const char *strName, char *dest, int destsize)
{
 
	string str;
    if (valuemap.find(strName) == valuemap.end())
	{  
		string errorstr;
        errorstr = string("Missing property \"") + string(strName) + string("\"!");
        throw(errorstr);
	}
    strncpy(dest, valuemap[strName].c_str(), destsize);
}

void CParticleEmitter::ReadProperty(map<string, string> &valuemap, 
                                   const char *strName, D3DXCOLOR &value)
{ 
	char buf[256]; char token[256]; float r,g,b,a;

    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));

    // color is in the form (R,G,B,A)
    PluckFirstField(buf, token, sizeof(token), "("); // skip first (
    PluckFirstField(buf, token, sizeof(token), ","); r = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ","); g = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ","); b = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ")"); a = (float)atof(token);

    value = D3DXCOLOR(r,g,b,a);
}

void CParticleEmitter::ReadProperty(map<string, string> &valuemap, 
                                   const char *strName, D3DXVECTOR3 &value)
{
    char buf[256]; char token[256]; float x,y,z;

    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));

    // vector is in the form (x,y,z)
    PluckFirstField(buf, token, sizeof(token), "("); // skip first (
    PluckFirstField(buf, token, sizeof(token), ","); x = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ","); y = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ")"); z = (float)atof(token);

    value = D3DXVECTOR3(x,y,z);
}

void CParticleEmitter::ReadProperty(map<string, string> &valuemap, 
                                   const char *strName, CMinMax<float> &value)
{
    char buf[256]; char token[256]; float minvalue,maxvalue;

    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));

    // minmax is in the form (min,max)
    PluckFirstField(buf, token, sizeof(token), "("); // skip first (
    PluckFirstField(buf, token, sizeof(token), ","); minvalue = (float)atof(token);
    PluckFirstField(buf, token, sizeof(token), ")"); maxvalue = (float)atof(token);

    value = CMinMax<float>(minvalue,maxvalue);
}

void CParticleEmitter::ReadProperty(map<string, string> &valuemap, 
                                   const char *strName, int &value)
{
    char buf[256];
    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));
    value = atoi(buf);
}

void CParticleEmitter::ReadProperty(map<string, string> &valuemap, 
                                   const char *strName, string &value)
{
    char buf[256];
    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));
    value = buf;
}

void CParticleEmitter::ReadBlendModeProperty(map<string, string> &valuemap, 
                                            const char *strName, int &value)
{
    char buf[256];
    GetRightSideFromValueMap(valuemap, strName, buf, sizeof(buf));
  
    for (int q=0; q < NUMBLENDINGMODES; q++) 
	{
        if (!_stricmp(m_BlendingModes[q].name, buf))
		{
            value = m_BlendingModes[q].mode;
            return;
        }
    }
    throw("Invalid Blending Mode.");
}
bool CParticleEmitter::Save(const char *strFilename)
{
	try
	{
		ofstream out(strFilename);
		out<<"[ParticleSystem 1.00]\n";

		out<<"Position"<<"=("<<m_vPos.x<<","<<m_vPos.y<<","<<m_vPos.z<<")"<<endl;
		out<<"Gravity"<<"=("<<m_vGravity.x<<","<<m_vGravity.y<<","<<m_vGravity.z<<")"<<endl;
		out<<"SpawnDir1"<<"=("<<m_vSpawnDir1.x<<","<<m_vSpawnDir1.y<<","<<m_vSpawnDir1.z<<")"<<endl;
		out<<"SpawnDir2"<<"=("<<m_vSpawnDir2.x<<","<<m_vSpawnDir2.y<<","<<m_vSpawnDir2.z<<")"<<endl;
		

		out<<"StartColor1"<<"=("<<m_StartColor1.r<<","<<m_StartColor1.g<<","<<m_StartColor1.b<<","<<m_StartColor1.a<<")"<<endl;
		out<<"StartColor2"<<"=("<<m_StartColor2.r<<","<<m_StartColor2.g<<","<<m_StartColor2.b<<","<<m_StartColor2.a<<")"<<endl;
		out<<"EndColor1"<<"=("<<m_EndColor1.r<<","<<m_EndColor1.g<<","<<m_EndColor1.b<<","<<m_EndColor1.a<<")"<<endl;
		out<<"EndColor2"<<"=("<<m_EndColor2.r<<","<<m_EndColor2.g<<","<<m_EndColor2.b<<","<<m_EndColor2.a<<")"<<endl;

		out<<"EmitRate"<<"=("<<m_fEmitRate.m_Min<<","<<m_fEmitRate.m_Max<<")"<<endl;
		out<<"EmitRadius"<<"=("<<m_vEmitRadius.x<<","<<m_vEmitRadius.y<<","<<m_vEmitRadius.z<<")"<<endl;
		out<<"Lifetime"<<"=("<<m_fLifetime.m_Min<<","<<m_fLifetime.m_Max<<")"<<endl;
		out<<"Size"<<"=("<<m_fSize.m_Min<<","<<m_fSize.m_Max<<")"<<endl;

		for (int q=0;q<NUMBLENDINGMODES;q++)
		{
			if(m_BlendingModes[q].mode == m_iSrcBlendMode) 
			{
				out<<"SrcBlend"<<"="<<m_BlendingModes[q].name<<endl;
				//return;
			}
		}
		for (int q=0;q<NUMBLENDINGMODES;q++)
		{
			if(m_BlendingModes[q].mode == m_iDestBlendMode) 
			{
				out<<"DestBlend"<<"="<<m_BlendingModes[q].name<<endl;
				//return;
			}
		}

		out<<"MaxParticles"<<"="<<m_iMaxParticles<<endl;
		out<<"Texture"<<"="<<m_strTexFilename.c_str()<<endl;

		out.close();
	} 
	catch(string e) 
	{
		m_strLastError = e;
		return(false);
	}
	catch(...) 
	{
		m_strLastError = "Unknown error.";
		return(false);
	}  
	return(true);
}
bool CParticleEmitter::Load(const char *strFilename)
{
	fstream file;
	file.open(strFilename);
	try
	{
		char buf[256];
		char leftside[256];
		char rightside[256];
		map<string, string> valuemap;
		// read first line - should be [ParticleSystem 1.00]
		file.getline(buf,sizeof(buf));
		if (strstr(buf, "[ParticleSystem 1.00]") == NULL) 
		{
			throw("Invalid Header (missing \"[ParticleSystem 1.00]\")");
		}
		// read the entire contents of file into a map.  If we read
		// more than 100 lines, we assume something's wrong and we bail out.
		int count=0;
		while (!file.eof()&& count < 1000)
		{
			// read a line
			file.getline(buf,sizeof(buf));
			// if there's something on that line,
			if (strlen(buf) >= 2)
			{
				// seperate that line into left and right side of = sign
				memset(leftside, 0, sizeof(leftside));
				memset(rightside, 0, sizeof(rightside));

				PluckFirstField(buf, leftside, sizeof(leftside), "=");
				PluckFirstField(buf, rightside, sizeof(rightside), "\n");
      
				// make sure the stuff to the left of the = sign isn't currently
				// in the map (if it is, throw an error).
				if (valuemap.find(leftside) != valuemap.end()) 
				{
					throw("Duplicate value encountered.");
				}

				// insert left and right sides into map.
				valuemap.insert(make_pair(string(leftside), string(rightside)));
			}

			// increment safety count
			count++;
		}

		if (count == 1000) 
		{      
			throw("File too big (most likely not a particle system file)");
		}

		ReadProperty(valuemap, "Position", m_vPos);
		ReadProperty(valuemap, "Gravity", m_vGravity);
		ReadProperty(valuemap, "SpawnDir1", m_vSpawnDir1);
		ReadProperty(valuemap, "SpawnDir2", m_vSpawnDir2);

		ReadProperty(valuemap, "StartColor1", m_StartColor1);
		ReadProperty(valuemap, "StartColor2", m_StartColor2);
		ReadProperty(valuemap, "EndColor1", m_EndColor1);
		ReadProperty(valuemap, "EndColor2", m_EndColor2);

		ReadProperty(valuemap, "EmitRate", m_fEmitRate);
		ReadProperty(valuemap, "EmitRadius", m_vEmitRadius);
		ReadProperty(valuemap, "Lifetime", m_fLifetime);
		ReadProperty(valuemap, "Size", m_fSize);

		ReadBlendModeProperty(valuemap, "SrcBlend", m_iSrcBlendMode);
		ReadBlendModeProperty(valuemap, "DestBlend", m_iDestBlendMode);

		ReadProperty(valuemap, "MaxParticles", m_iMaxParticles);
		ReadProperty(valuemap, "Texture", m_strTexFilename);

		file.close(); 
	}
  
	catch(const char *e)
	{
		file.close(); 
		m_strLastError = e;
		return(false);
	}
	catch(...) 
	{  
		file.close();  
		m_strLastError = "Unknown error.";  
		return(false);
	}
	return(true);
}
/****************************************************************************
 *                                                                          *
 * PluckFirstField: pulls the first "field" off of str and puts it in dest  *
 * (uses delim as a delimiter)                                              *
 *                                                                          *
 ****************************************************************************/
void CParticleEmitter::PluckFirstField(char *str, char *dest, int maxlen,const char *delim)
{
	char *endpos;
    int p;

    if (!strlen(delim)) 
	{
		strcpy_s(dest, strlen(str)+1,str); 
		return;
	}
    endpos = strstr(str, delim);
    if (!endpos) 
	{ 
		strcpy_s(dest, strlen(str)+1,str); 
		return; 
	}
    p =(int)(endpos - str);

    memset(dest, 0, maxlen);
    memcpy(dest, str, p);

    // pluck it off of str...
    strcpy(str, &str[p+strlen(delim)]);//-
}