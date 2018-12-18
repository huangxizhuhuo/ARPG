#ifndef CORE_HELP_H
#define CORE_HELP_H

#include <d3dx9.h>

typedef struct 
{ 
	D3DXVECTOR3 position; // The position 
	D3DCOLOR    color;    // The color
	FLOAT       tu, tv;   // The texture coordinates
} VERTEX_XYZ_DIFFUSE_TEX1;

#define D3DFVF_XYZ_DIFFUSE_TEX1 (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

HRESULT CreateQuad(LPDIRECT3DVERTEXBUFFER9 *pVB, D3DPOOL pool, float fSize, 
                   DWORD dwColor, LPDIRECT3DDEVICE9 pDev,
                   float fTexTileX=1.0f, float fTexTileY=1.0f);

HRESULT SetupOrthoProjForRenderCopy(LPDIRECT3DDEVICE9 pDev, int iWidth, int iHeight);

HRESULT RenderCopy(LPDIRECT3DTEXTURE9 ptexSource, LPDIRECT3DTEXTURE9 ptexDest,
                   int iDestWidth, int iDestHeight, LPDIRECT3DDEVICE9 pDev,
                   LPDIRECT3DVERTEXBUFFER9 pvbQuad);

HRESULT SmoothRenderCopy(LPDIRECT3DTEXTURE9 ptexSource, LPDIRECT3DTEXTURE9 ptexDest, 
                         int iDestWidth, int iDestHeight, LPDIRECT3DDEVICE9 pDev, 
                         LPDIRECT3DVERTEXBUFFER9 pvbQuad);

int GetLowestPowerOf2(int n);

inline float Noise2D(int x, int y)
{
	int n = x + y * 57;
    n = (n<<13) ^ n;
    return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);    
}
 
inline float Noise3D(int x, int y, int z)
{
	int n = x + y * 57 + z * 131;
    n = (n<<13) ^ n;
    return (1.0f-((n*(n*n*15731+789221)+1376312589)&0x7fffffff)*0.000000000931322574615478515625f);    
}

inline int RandomNumber(int iMin, int iMax)
{
	if (iMin == iMax) return(iMin);
    return((rand() % (abs(iMax-iMin)+1))+iMin);
}

inline void D3DXColorTo255RGBA(D3DXCOLOR color,unsigned char &cr, unsigned char &cg, unsigned char &cb, unsigned char &ca)
{
    if (color.r > 1.0f) color.r = 1.0f; if (color.r < 0.0f) color.r = 0.0f;
    if (color.g > 1.0f) color.g = 1.0f; if (color.g < 0.0f) color.g = 0.0f;
    if (color.b > 1.0f) color.b = 1.0f; if (color.b < 0.0f) color.b = 0.0f;
    if (color.a > 1.0f) color.a = 1.0f; if (color.a < 0.0f) color.a = 0.0f;

    int r = (int)floor(color.r * 255.0f);
    int g = (int)floor(color.g * 255.0f);
    int b = (int)floor(color.b * 255.0f);
    int a = (int)floor(color.a * 255.0f);
  
    cr = (unsigned char)r;
    cg = (unsigned char)g;
    cb = (unsigned char)b;
    ca = (unsigned char)a;
}

inline D3DXCOLOR RGBAToD3DXColor(unsigned char cr, unsigned char cg, unsigned char cb, unsigned char ca)
{
    D3DXCOLOR c;
    c.r = (float)cr / 255.0f;
    c.g = (float)cg / 255.0f;
    c.b = (float)cb / 255.0f;
    c.a = (float)ca / 255.0f;
    return c;
}

HRESULT CreateVertexFrame(LPDIRECT3DVERTEXBUFFER9 *pVB, LPDIRECT3DINDEXBUFFER9 *pIB,                        
						  D3DPOOL pool, float fTotalSize, DWORD dwColor, 
						  int iNumVerticesX, int iNumVerticesY, LPDIRECT3DDEVICE9 pDev);

HRESULT CreateVertexGrid(LPDIRECT3DVERTEXBUFFER9 *pVB, LPDIRECT3DINDEXBUFFER9 *pIB,                         
						 D3DPOOL pool, float fTotalSize, DWORD dwColor,           
						 int iNumVerticesX, int iNumVerticesY, LPDIRECT3DDEVICE9 pDev);

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

template <class TArrayElement, int iNumElements> 
class CRecyclingArray 
{
public:
	CRecyclingArray()
	{  
		for(int q=0; q < iNumElements; q++) 
		{   
			m_bAlive[q] = false;
		}
		m_iUsedElements = 0;
		m_iLastNew = 0;
	}
	virtual ~CRecyclingArray() { }
	TArrayElement* New()
	{
		// assert that we have space for this one
		if (GetNumFreeElements() < 1) 
			throw(std::out_of_range("CRecyclingArray::New: too many objects!"));
		// find first element not in use.  as an optimization, we start at   
		// the position that was allocated last, in the hopes that the next position will be free.
		int i = m_iLastNew;
		for (int q=0; q < iNumElements; q++)
		{ 
			if (!m_bAlive[i])
			{
				// we've found our free spot!  use it!   
				break;  
			} 
			else 
			{    
				i++; 
				if (i >= iNumElements) 
					i = 0;
			}
		}
		if (m_bAlive[i]) 
		{
			// huh? if we got here, there are no free elements in the list... yet 
			// GetNumFreeElements didn't tell us that in the beginning.  Logic error.
			throw(std::logic_error("TArrayElement::New(): internal logic error."));
		}
		// clear it
		m_aElements[i] = m_NewTemplate;
		// increment used count
		m_iUsedElements++;
		m_bAlive[i] = true;
		m_iLastNew = i;
		// return it
		return &m_aElements[i];
	}
	bool Delete(int index)
	{  
		if (index < 0 || index >= iNumElements || !m_bAlive[index]) 
			return false;
		// don't actually delete element at index;
		// just mark it free.
		m_bAlive[index] = false;   
		m_iUsedElements--;   
		return true;
	}
	bool Delete(TArrayElement* elem) 
	{   
		if (m_iUsedElements == 0) return(false);  
		int iElem = reinterpret_cast<int>(elem);   
		iElem -= reinterpret_cast<int>(&m_aElements[0]); 
		int index = iElem / sizeof(TArrayElement);  
		if (index < 0 || index >= iNumElements || !m_bAlive[index])
			return false;  
		m_bAlive[index] = false;
		m_iUsedElements--; 
		return true;
	}
	int GetNumFreeElements(void) { return(iNumElements-GetNumUsedElements()); }
	int GetNumUsedElements(void) { return(m_iUsedElements); }
	int GetTotalElements(void)   { return(iNumElements); }
	TArrayElement& GetAt(int index) { return(m_aElements[index]); }
	bool IsAlive(int index) { return(m_bAlive[index]); }
	void DeleteAll(void)
	{  
		for (int q=0; q < iNumElements; q++)
		{   
			m_bAlive[q] = false;
		}
		m_iUsedElements = 0;
		m_iLastNew = 0;
	}
protected:
	int m_iUsedElements; 
	TArrayElement m_aElements[iNumElements];  
	bool m_bAlive[iNumElements];
	int m_iLastNew;
	TArrayElement m_NewTemplate;
};
#endif