#ifndef PARTICLE_H
#define PARTICLE_H

#include "../Core/Core_Global.h"

class CParticle
{
public:

	CParticle() { m_fSize = 1.0f; m_fLifetime = 1.0f; m_fAge = 0.0f; }
	virtual ~CParticle() { }

	float m_fSize;
	float m_fLifetime;
	float m_fAge;

	D3DXCOLOR m_Color;
	D3DXCOLOR m_ColorStep;
  
	D3DXVECTOR3 m_vPos; // current position of particle
	D3DXVECTOR3 m_vDir;

	// this function is inline not because it's small, but because it's only called
	// in one place (CParticleEmitter.Update()).  This way we can also dodge the
	// performance hit associated with calling a function many times (remember, we're
	// in a for loop in CParticleEmitter.Update()).

	// your needs may vary.  For example, if you can afford the speed hit, it might
	// be cool to make this a virtual function.

	inline bool Update(float fTimeDelta)
	{
		// age the particle
		m_fAge += fTimeDelta;
		// if this particle's age is greater than it's lifetime, it dies.
		if (m_fAge >= m_fLifetime) 
			return false; // dead!
		// move particle
		m_vPos += m_vDir * fTimeDelta;
    
		return true; // particle stays alive
	}

};
#endif
