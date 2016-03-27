### 粒子的定义 ###
粒子是dx9的基本图元，叫点精灵：Point Sprite。点精灵也没啥特别，只是一个概念而已，在这个概率里，你按照MS给你指定的流程去渲染buffer就认为你做了点精灵操作了。


### 创建 ###
_[D3DUSAGE\_POINTS](D3DUSAGE_POINTS.md)_
```
hr = device->CreateVertexBuffer(_vbSize * sizeof(Particle),
	D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
	Particle::FVF,
	D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
	&_vb,
	0);
}
```


### FVF如下 ###
```
const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
```

### 打开渲染状态 ###
_D3DRS\_POINTSPRITEENABLE_
```
_device->SetRenderState(D3DRS_LIGHTING, false);
_device->SetRenderState(D3DRS_POINTSPRITEENABLE, true);
_device->SetRenderState(D3DRS_POINTSCALEENABLE, true); 
_device->SetRenderState(D3DRS_POINTSIZE, d3d::FtoDw(_size));
_device->SetRenderState(D3DRS_POINTSIZE_MIN, d3d::FtoDw(0.0f));

// control the size of the particle relative to distance
_device->SetRenderState(D3DRS_POINTSCALE_A, d3d::FtoDw(0.0f));
_device->SetRenderState(D3DRS_POINTSCALE_B, d3d::FtoDw(0.0f));
_device->SetRenderState(D3DRS_POINTSCALE_C, d3d::FtoDw(1.0f));        
// use alpha from texture
_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
```

### 渲染完了，需要还原渲染状态到一般情况 ###
```
_device->SetRenderState(D3DRS_LIGHTING,          true);
_device->SetRenderState(D3DRS_POINTSPRITEENABLE, false);
_device->SetRenderState(D3DRS_POINTSCALEENABLE,  false);
_device->SetRenderState(D3DRS_ALPHABLENDENABLE,  false);
```

### 执行渲染 ###
_D3DPT\_POINTLIST_
```
_device->SetFVF(Particle::FVF);
_device->SetStreamSource(0, _vb, 0, sizeof(Particle));
_device->DrawPrimitive(D3DPT_POINTLIST,_vbOffset,_vbBatchSize);
```

### 执行帧渲染还有不少学问的 ###
**其一:粒子生命周期。你得指定一个例子生命周期，不然例子不会消失。在帧渲染里，渲染开始之前，你搞个update方法进行相关操作，然后再渲染。这样的好处是update里还可以做其他操作。**

```
void Firework::update(float timeDelta)
 {
    std::list<Attribute>::iterator i;

    for(i = _particles.begin(); i != _particles.end(); i++)
    {
        // only update living particles
        if( i->_isAlive )
        {
            i->_position += i->_velocity * timeDelta;

            i->_age += timeDelta;

            if(i->_age > i->_lifeTime) // kill 
                i->_isAlive = false;
        }
    }
}

```


**其二:粒子属性。VB里只包含了例子的坐标和颜色信息，你得给每个例子指定一个属性，这样在update的时候可以修改属性，例如例子的颜色，位置，年龄，婚否，在世否。然后在渲染的时候去访问VB，并重新给VB的元素赋值。大概像这样：**
```
Particle* v = 0;

_vb->Lock(
		  _vbOffset    * sizeof( Particle ),
		  _vbBatchSize * sizeof( Particle ),
		  (void**)&v,
		  _vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

std::list<Attribute>::iterator i;
for(i = _particles.begin(); i != _particles.end(); i++)
{
	if( i->_isAlive )
	{
		//
		// Copy a batch of the living particles to the
		// next vertex buffer segment
		//
		v->_position = i->_position;
		v->_color    = (D3DCOLOR)i->_color;
		v++; // next element;

	}    
}

_vb->Lock(

```

**其三:分批渲染。粒子可能有好多万，你不能炫富。你得分批渲染，可以类似这样做：**

```xml

批处理计数=0;
for 所有顶点的遍历
打开VB缓存
修改顶点属性
批处理计数++
如果批处理计数>=批次处理最大数
关闭顶点缓存
渲染这个批次的顶点
顶点偏移指针进行偏移，指导下一批次的开始
打开VB缓存
批处理计数=0
end 所有定点的遍历

//渲染余下不够批次大小的顶点，这些定点正好是批次处理最大数的余数
关闭顶点缓存
if 批处理计数!=0
DrawPrimitive渲染
```

以上其实就是一个非常简单的将一个巨量元素进行分批次处理的逻辑。不知道是不是OGRE中的BATCH处理的原型。反正3D API没多聪明，你将巨量元素交给它，它是不会进行智能处理的。这样也好，容易理解多了。也方便我们智能处理。

**其四:纹理。**
```
D3DRS_POINTSPRITEENABLE
```
这个影响默认贴图方式。当设置为true时，贴图会完全贴上Point Sprites粒子；如果设置为false,那么，当粒子的顶点结构中有UV坐标时贴图才有效，并且根据不同粒子的UV坐标贴到不同的位置。默认是FALSE。

D3DRS\_POINTSCALEENABLE
控制了粒子是否执行3D视图变换。如果不变换，粒子不管多远都一样大，有点像文字那样大小固定了。默认是FALSE。


粒子渲染的api其实非常简单，渲染逻辑也不复杂，简单来说就是在帧渲染中用批处理思想执行一个update操作和一个render操作。update交给cpu更新啊，render交给gpu处理啊，cpu和gpu你都上伤起啊。



&lt;hr&gt;


## ID3DXSprite使用步骤 ##
SOURCE:http://code.google.com/p/3dlearn/source/browse/trunk/DirectX/DragonBook2ShaderCode/Chapter%205/PageFlipDemo/PageFlipDemo.cpp

### 创建 ###

```
ID3DXSprite* mSprite;
HR(D3DXCreateSprite(gd3dDevice, &mSprite));

```

### 更新 ###
```
...
HR(mSprite->OnLostDevice());

...
HR(mSprite->OnResetDevice());

```
### 渲染 ###
```
HR(mSprite->Begin(D3DXSPRITE_OBJECTSPACE|D3DXSPRITE_DONOTMODIFY_RENDERSTATE));

// Turn on alpha blending.
HR(gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true));

// Don't move explosion--set identity matrix.
D3DXMATRIX M;
D3DXMatrixIdentity(&M);
HR(mSprite->SetTransform(&M));
HR(mSprite->Draw(mFrames, &R, &mSpriteCenter, 0, D3DCOLOR_XRGB(255, 255, 255)));
HR(mSprite->End());

// Turn off alpha blending.
HR(gd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false));

mGfxStats->display();

HR(gd3dDevice->EndScene());

// Present the backbuffer.
HR(gd3dDevice->Present(0, 0, 0, 0));
```

### 销毁 ###
```
#define ReleaseCOM(x) { if(x){ x->Release();x = 0; } }
ReleaseCOM(mSprite);

```


_补充_
```
#define HR(x)                                      \
{                                                  \
	HRESULT hr = x;                                \
	if(FAILED(hr))                                 \
{                                              \
	DXTrace(__FILE__, __LINE__, hr, #x, TRUE); \
}                                              \
```