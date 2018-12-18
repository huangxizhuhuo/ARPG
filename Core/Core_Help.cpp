#include "Core_Help.h" 

HRESULT CreateQuad(LPDIRECT3DVERTEXBUFFER9 *pVB, D3DPOOL pool, float fSize, 
                   DWORD dwColor, LPDIRECT3DDEVICE9 pDev,
                   float fTexTileX, float fTexTileY)
{
	HRESULT hr;
	if(FAILED(hr = pDev->CreateVertexBuffer(6*sizeof(VERTEX_XYZ_DIFFUSE_TEX1),               
		0, D3DFVF_XYZ_DIFFUSE_TEX1, pool, pVB,NULL)))  
		return hr; 
	VERTEX_XYZ_DIFFUSE_TEX1 *pVertices; 
	float fSizeDiv2 = fSize/2; 
	if( FAILED( hr = (*pVB)->Lock( 0, 6*sizeof(VERTEX_XYZ_DIFFUSE_TEX1), (void**)&pVertices, 0 ) ) )   
		return hr;
	// first triangle
	pVertices[0].position = D3DXVECTOR3(-fSizeDiv2, fSizeDiv2, 0.0f); 
	pVertices[0].color    = dwColor;  
	pVertices[0].tu       = 0.0f; 
	pVertices[0].tv       = 0.0f;
  
	pVertices[1].position = D3DXVECTOR3(fSizeDiv2, fSizeDiv2, 0.0f); 
	pVertices[1].color    = dwColor;
	pVertices[1].tu       = fTexTileX;
	pVertices[1].tv       = 0.0f;
 
	pVertices[2].position = D3DXVECTOR3(fSizeDiv2, -fSizeDiv2, 0.0f); 
	pVertices[2].color    = dwColor;
	pVertices[2].tu       = fTexTileX;
	pVertices[2].tv       = fTexTileY;

	// second triangle
	pVertices[3].position = D3DXVECTOR3(-fSizeDiv2, fSizeDiv2, 0.0f);
	pVertices[3].color    = dwColor;
	pVertices[3].tu       = 0.0f;
	pVertices[3].tv       = 0.0f;

	pVertices[4].position = D3DXVECTOR3(fSizeDiv2, -fSizeDiv2, 0.0f);
	pVertices[4].color    = dwColor;
	pVertices[4].tu       = fTexTileX;
	pVertices[4].tv       = fTexTileY;

	pVertices[5].position = D3DXVECTOR3(-fSizeDiv2, -fSizeDiv2, 0.0f);
	pVertices[5].color    = dwColor; 
	pVertices[5].tu       = 0.0f;
	pVertices[5].tv       = fTexTileY;

	if( FAILED( hr = (*pVB)->Unlock() ) )
		return hr; 
	return S_OK;
}

HRESULT SetupOrthoProjForRenderCopy(LPDIRECT3DDEVICE9 pDev, int iWidth, int iHeight)
{
	// set up orthogonal projection matrix, so that one texel = one pixel in the final
	// image, with no perspective correction.
	D3DXMATRIX projmat;
	D3DXMatrixOrthoLH(&projmat, (float)iWidth, (float)iHeight, 0.0, 100.0); 
	pDev->SetTransform( D3DTS_PROJECTION, &projmat );
	// set up world matrix so that it rotates the quad slightly. 
	D3DXMATRIX worldmattemp, worldmat, transmat; 
	D3DXMatrixScaling(&worldmattemp, (float)iWidth/2.0f, (float)iHeight/2.0f, 1.0);
	// move the quad left and up 0.5 units, so that the texels are perfectly
	// centered on the screen pixels.
	D3DXMatrixMultiply(&worldmat, &worldmattemp, D3DXMatrixTranslation(&transmat, -0.5f, -0.5f, 0));
	pDev->SetTransform( D3DTS_WORLD, &worldmat );
	// set up view matrix
	D3DXMATRIX viewmat;
	D3DXMatrixIdentity(&viewmat);
	pDev->SetTransform( D3DTS_VIEW, &viewmat );
	return S_OK;
}

HRESULT RenderCopy(LPDIRECT3DTEXTURE9 ptexSource, LPDIRECT3DTEXTURE9 ptexDest,
                   int iDestWidth, int iDestHeight, LPDIRECT3DDEVICE9 pDev,
                   LPDIRECT3DVERTEXBUFFER9 pvbQuad)
{
	HRESULT hr; 
	// get the current rendering target (we'll set it back later)
	LPDIRECT3DSURFACE9 pOldRenderTarget;
	pDev->GetRenderTarget(0,&pOldRenderTarget);
	// get surface interfaces
	LPDIRECT3DSURFACE9 psurfDest;
	ptexDest->GetSurfaceLevel(0, &psurfDest);
	// set new rendering target & clear
	pDev->SetRenderTarget(0,psurfDest);
	pDev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x000000, 1.0f, 0); 
	// turn off z buffering
	pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	// set up texture stages for simple texture stage copy
	pDev->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDev->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDev->SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_DISABLE); 
	pDev->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
	SetupOrthoProjForRenderCopy(pDev, iDestWidth, iDestHeight);
	// set the source texture active
	pDev->SetTexture(0, ptexSource);
	// begin rendering the scene
	if (FAILED(hr = pDev->BeginScene())) 
		return hr;
	pDev->SetStreamSource( 0, pvbQuad, 0,sizeof(VERTEX_XYZ_DIFFUSE_TEX1));
	pDev->SetFVF( D3DFVF_XYZ_DIFFUSE_TEX1 );
 	// this "blits" the texture
	pDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
	// end scene
	pDev->EndScene();
	// release the dest surface 
	psurfDest->Release();
	// set the rendering target back to the old target. 
	pDev->SetRenderTarget(0,pOldRenderTarget);
	pDev->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE); 
	pOldRenderTarget->Release();
	return S_OK;
}

HRESULT SmoothRenderCopy(LPDIRECT3DTEXTURE9 ptexSource, LPDIRECT3DTEXTURE9 ptexDest, 
                         int iDestWidth, int iDestHeight, LPDIRECT3DDEVICE9 pDev, 
                         LPDIRECT3DVERTEXBUFFER9 pvbQuad)
{ 
	// no alpha blending or lighting
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
	pDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	// turn on bilinear filtering (this is what "smooths the noise") 
	pDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); 
	pDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	// render-copy the texture  
	return RenderCopy(ptexSource, ptexDest, iDestWidth, iDestHeight, pDev, pvbQuad);
}

int GetLowestPowerOf2(int n)
{
	int lowest = 1;
	while(lowest < n)
		lowest <<= 1;
	return lowest;
}

HRESULT CreateVertexFrame(LPDIRECT3DVERTEXBUFFER9 *pVB, LPDIRECT3DINDEXBUFFER9 *pIB,
                         D3DPOOL pool, float fTotalSize, DWORD dwColor, 
                         int iNumVerticesX, int iNumVerticesY, LPDIRECT3DDEVICE9 pDev)
{   
	HRESULT hr;
	// create and fill vertex buffer
	{
		if(FAILED(hr = pDev->CreateVertexBuffer((((iNumVerticesX-1)*2)+
			((iNumVerticesY-1)*2)+2)*sizeof(VERTEX_XYZ_DIFFUSE_TEX1), 
			0, D3DFVF_XYZ_DIFFUSE_TEX1, pool, pVB,NULL)))
			return hr;
		VERTEX_XYZ_DIFFUSE_TEX1 *pVertices;
		float fSizeDiv2 = fTotalSize/2;
		if( FAILED( hr = (*pVB)->Lock( 0, (((iNumVerticesX-1)*2)+
			((iNumVerticesY-1)*2)+1)*sizeof(VERTEX_XYZ_DIFFUSE_TEX1), 
			(void**)&pVertices, 0 ) ) )
			return hr;
		// top row   
		for (int x=0; x < iNumVerticesX; x++) 
		{
			pVertices->position = D3DXVECTOR3(
				(((float)x/(float)(iNumVerticesX-1))*fTotalSize)-fSizeDiv2, 
				fSizeDiv2,0.0f);
			pVertices->color    = dwColor; 
			pVertices->tu       = (float)x/(float)(iNumVerticesX-1);    
			pVertices->tv       = 0.0f;  
			pVertices++;  
		}     
		// right row 
		for (int y=1; y < iNumVerticesY; y++) 
		{   
			pVertices->position = D3DXVECTOR3(      
				fSizeDiv2,   
				(((float)(iNumVerticesY-1-y)/(float)(iNumVerticesY-1))*fTotalSize)-fSizeDiv2,        
				0.0f);   
			pVertices->color    = dwColor;  
			pVertices->tu       = 1.0f;   
			pVertices->tv       = (float)y/(float)(iNumVerticesY-1);   
			pVertices++;
		}  
		// bottom row  
		for (int x=iNumVerticesX-1; x >= 1; x--) 
		{    
			pVertices->position = D3DXVECTOR3(       
				(((float)x/(float)(iNumVerticesX-1))*fTotalSize)-fSizeDiv2,       
				-fSizeDiv2,        
				0.0f);
			pVertices->color    = dwColor;    
			pVertices->tu       = (float)x/(float)(iNumVerticesX-1);  
			pVertices->tv       = 1.0f;   
			pVertices++;
		}    
		// left row  
		for (int y=iNumVerticesY-1; y >= 1; y--) 
		{  
			pVertices->position = D3DXVECTOR3(       
				-fSizeDiv2,         
				(((float)(iNumVerticesY-1-y)/(float)(iNumVerticesY-1))*fTotalSize)-fSizeDiv2,        
				0.0f); 
			pVertices->color    = dwColor; 
			pVertices->tu       = 0.0f;    
			pVertices->tv       = (float)y/(float)(iNumVerticesY-1);   
			pVertices++;  
		} 
		// last point in exact center  
		pVertices->position = D3DXVECTOR3(0.0f,0.0f, 0.0f);
		pVertices->color    = dwColor;  
		pVertices->tu       = 0.5f;  
	    pVertices->tv       = 0.5f;  
     }
    // create index buffer
    {   
		if(FAILED(hr = pDev->CreateIndexBuffer(        
			(((iNumVerticesX-1)*2)+((iNumVerticesY-1)*2))*3*2, // *3 (indicies per tri) * 2 (bytes per index)     
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,      
			D3DPOOL_MANAGED, pIB,NULL)))   
			return hr;
		// lock and fill index buffer
		WORD *pIndices;   
		if(FAILED(hr = (*pIB)->Lock(0, (((iNumVerticesX-1)*2)+((iNumVerticesY-1)*2))*3*2, (void**)&pIndices,0)))   
			return hr;  
		// the triangles are arranged in a fan pattern...    
		// probably could have used tri fans here but GeForce class cards like tri lists better
		WORD *pIndex = pIndices;
		for (int q=0; q < (((iNumVerticesX-1)*2)+((iNumVerticesY-1)*2)); q++) 
		{ 
			// first triangle    
			*(pIndex++) = q;    
			*(pIndex++) = ((iNumVerticesX-1)*2)+((iNumVerticesY-1)*2);   
			*(pIndex++) = ((q+1 == ((iNumVerticesX-1)*2)+((iNumVerticesY-1)*2)) ? 0 : q+1);
		}
		if(FAILED(hr = (*pIB)->Unlock()))
			return hr;  
		if(FAILED(hr = (*pVB)->Unlock())) 
			return hr;
	}
	return S_OK;
}

HRESULT CreateVertexGrid(LPDIRECT3DVERTEXBUFFER9 *pVB, LPDIRECT3DINDEXBUFFER9 *pIB,                        
						 D3DPOOL pool, float fTotalSize, DWORD dwColor,                         
						 int iNumVerticesX, int iNumVerticesY, LPDIRECT3DDEVICE9 pDev)
{  
	HRESULT hr;
	// create and fill vertex buffer 
	{
		if(FAILED(hr = pDev->CreateVertexBuffer(iNumVerticesX*iNumVerticesY*sizeof(VERTEX_XYZ_DIFFUSE_TEX1),                    
			0, D3DFVF_XYZ_DIFFUSE_TEX1, pool, pVB,NULL))) 			
			return hr;
		VERTEX_XYZ_DIFFUSE_TEX1 *pVertices; 
		float fSizeDiv2 = fTotalSize/2;
		if( FAILED( hr = (*pVB)->Lock( 0, iNumVerticesX*iNumVerticesY*sizeof(VERTEX_XYZ_DIFFUSE_TEX1), (void**)&pVertices, 0 ) ) )       
			return hr;
		for(int x=0; x < iNumVerticesX; x++) 
		{  
			for (int y=0; y < iNumVerticesY; y++)
			{      
				pVertices[(y*iNumVerticesX)+x].position = D3DXVECTOR3(       
					(iNumVerticesX > 1) ? (((float)x/(float)(iNumVerticesX-1))*fTotalSize)-fSizeDiv2 : 0,         
					(iNumVerticesY > 1) ? (((float)(iNumVerticesY-1-y)/(float)(iNumVerticesY-1))*fTotalSize)-fSizeDiv2 : 0,         
					0.0f);
				pVertices[(y*iNumVerticesX)+x].color  = dwColor;     
				pVertices[(y*iNumVerticesX)+x].tu     = (float)x/(float)(iNumVerticesX-1);      
				pVertices[(y*iNumVerticesX)+x].tv     = (float)y/(float)(iNumVerticesY-1);   
			} 
		}
	}
	// create index buffer 
	{
		if(FAILED(hr = pDev->CreateIndexBuffer(     
			iNumVerticesX*iNumVerticesY*2*3*2, // *2 (tris) *3 (indicies per tri) * 2 (bytes per index)       
			D3DUSAGE_WRITEONLY, D3DFMT_INDEX16,    
			D3DPOOL_MANAGED, pIB,NULL)))
			return hr;
		// lock and fill index buffer
		WORD *pIndices;
		if(FAILED(hr = (*pIB)->Lock(0, iNumVerticesX*iNumVerticesY*2*3*2, (void**)&pIndices, 0)))
			return hr;
		WORD *pIndex = pIndices;  
		for (int x=0; x < iNumVerticesX-1; x++) 
		{
			for (int y=0; y < iNumVerticesY-1; y++)
			{
				// first triangle     
				*(pIndex++) = ((y)*iNumVerticesX)+x;     
				*(pIndex++) = ((y)*iNumVerticesX)+x+1;      
				*(pIndex++) = ((y+1)*iNumVerticesX)+x+1;      
				// second triangle  
				*(pIndex++) = ((y)*iNumVerticesX)+x;        
				*(pIndex++) = ((y+1)*iNumVerticesX)+x+1;       
				*(pIndex++) = ((y+1)*iNumVerticesX)+x;   
			}
		}
		if(FAILED(hr = (*pIB)->Unlock()))
			return hr;   
		if(FAILED(hr = (*pVB)->Unlock()))
			return hr;
	}
	return S_OK;
}
