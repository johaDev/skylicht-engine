#include "pch.h"
#include "CViewInit.h"
#include "CViewDemo.h"

#include "ViewManager/CViewManager.h"
#include "Context/CContext.h"

#include "Primitive/CPlane.h"
#include "SkyDome/CSkyDome.h"

CViewInit::CViewInit() :
	m_initState(CViewInit::DownloadBundles),
	m_getFile(NULL),
	m_downloaded(0),
	m_bakeSHLighting(true)
{

}

CViewInit::~CViewInit()
{

}

io::path CViewInit::getBuiltInPath(const char* name)
{
	return getApplication()->getBuiltInPath(name);
}

void CViewInit::onInit()
{
	CBaseApp* app = getApplication();
	app->showDebugConsole();
	app->getFileSystem()->addFileArchive(getBuiltInPath("BuiltIn.zip"), false, false);

	CShaderManager* shaderMgr = CShaderManager::getInstance();
	shaderMgr->initBasicShader();
	shaderMgr->initSGForwarderShader();
	shaderMgr->initSGDeferredShader();

	CGlyphFreetype* freetypeFont = CGlyphFreetype::getInstance();
	freetypeFont->initFont("Segoe UI Light", "BuiltIn/Fonts/segoeui/segoeuil.ttf");

	// init basic gui
	CZone* zone = CContext::getInstance()->initScene()->createZone();
	m_guiObject = zone->createEmptyObject();
	CCanvas* canvas = m_guiObject->addComponent<CCanvas>();

	// load font
	m_font = new CGlyphFont();
	m_font->setFont("Segoe UI Light", 25);

	// create text
	m_textInfo = canvas->createText(m_font);
	m_textInfo->setTextAlign(EGUIHorizontalAlign::Center, EGUIVerticalAlign::Middle);
	m_textInfo->setText(L"Init assets");

	// create gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);
	CContext::getInstance()->setGUICamera(guiCamera);
}

void CViewInit::initScene()
{
	CBaseApp* app = getApplication();

	// create a scene
	CScene* scene = CContext::getInstance()->getScene();
	CZone* zone = scene->getZone(0);

	// camera
	CGameObject* camObj = zone->createEmptyObject();
	camObj->addComponent<CCamera>();
	camObj->addComponent<CEditorCamera>()->setMoveSpeed(2.0f);
	camObj->addComponent<CFpsMoveCamera>()->setMoveSpeed(1.0f);

	CCamera* camera = camObj->getComponent<CCamera>();
	camera->setPosition(core::vector3df(0.0f, 1.8f, 3.0f));
	camera->lookAt(core::vector3df(0.0f, 1.0f, 0.0f), core::vector3df(0.0f, 1.0f, 0.0f));

	// gui camera
	CGameObject* guiCameraObject = zone->createEmptyObject();
	CCamera* guiCamera = guiCameraObject->addComponent<CCamera>();
	guiCamera->setProjectionType(CCamera::OrthoUI);

	// sky
	ITexture* skyDomeTexture = CTextureManager::getInstance()->getTexture("Common/Textures/Sky/PaperMill.png");
	if (skyDomeTexture != NULL)
	{
		CSkyDome* skyDome = zone->createEmptyObject()->addComponent<CSkyDome>();
		skyDome->setData(skyDomeTexture, SColor(255, 255, 255, 255));
	}

	// reflection probe
	CGameObject* reflectionProbeObj = zone->createEmptyObject();
	CReflectionProbe* reflection = reflectionProbeObj->addComponent<CReflectionProbe>();
	reflection->loadStaticTexture("Common/Textures/Sky/PaperMill");

	// 3D grid
	CGameObject* grid = zone->createEmptyObject();
	CPlane* plane = grid->addComponent<CPlane>();
	plane->getMaterial()->changeShader("BuiltIn/Shader/SpecularGlossiness/Deferred/MetersGrid.xml");
	grid->getTransformEuler()->setScale(core::vector3df(50.0f, 1.0f, 50.0f));


	// lighting
	CGameObject* lightObj = zone->createEmptyObject();
	CDirectionalLight* directionalLight = lightObj->addComponent<CDirectionalLight>();

	CTransformEuler* lightTransform = lightObj->getTransformEuler();
	lightTransform->setPosition(core::vector3df(2.0f, 2.0f, 2.0f));

	core::vector3df direction = core::vector3df(4.0f, -6.0f, -4.5f);
	lightTransform->setOrientation(direction, Transform::Oy);

	CAnimationManager* animManager = CAnimationManager::getInstance();
	CAnimationClip* clip1 = animManager->loadAnimation("SampleModels/MixamoCharacter/Hip_Hop_Dancing.dae");
	CAnimationClip* clip2 = animManager->loadAnimation("SampleModels/MixamoCharacter/Samba_Dancing.dae");

	// skinned mesh
	CEntityPrefab* prefab = CMeshManager::getInstance()->loadModel("SampleModels/MixamoCharacter/Ch17_nonPBR.dae", "SampleModels/MixamoCharacter/textures");
	if (prefab != NULL)
	{
		ArrayMaterial material = CMaterialManager::getInstance()->initDefaultMaterial(prefab);

		// create character
		CGameObject* character = zone->createEmptyObject();

		// load skinned mesh character
		CRenderMesh* renderMesh = character->addComponent<CRenderMesh>();
		renderMesh->initFromPrefab(prefab);
		renderMesh->initMaterial(material);

		// apply animation to character
		CAnimationController* animController = character->addComponent<CAnimationController>();
		CSkeleton* skeleton = animController->createSkeleton();
		skeleton->setAnimation(clip1, true);

		// get bone map transform
		std::map<std::string, int> boneMap;
		skeleton->getBoneIdMap(boneMap);

		int totalFrames = (int)(clip1->Duration * 60.0f); // 60fps
		int numBones = (int)boneMap.size();

		core::matrix4* transforms = new core::matrix4[numBones];
		core::matrix4* animationData = new core::matrix4[totalFrames * numBones];

		// build the matrix animations
		// 
		//  [anim1]   frame0    frame1    frame2    ...
		//  bone1
		//  bone2
		//  bone3
		//  ...
		//  [anim2]   frame0    frame1    frame2    ...
		//  bone1
		//  bone2
		//  bone3
		//  ...

		for (int i = 0; i < totalFrames; i++)
		{
			float t = i / (float)totalFrames;
			skeleton->simulateTransform(t * clip1->Duration, core::IdentityMatrix, transforms, numBones);

			for (int j = 0; j < numBones; j++)
			{
				animationData[j * totalFrames + i] = transforms[j];
			}
		}

		// remove current charracter
		character->remove();

		// create gpu anim character
		character = zone->createEmptyObject();
		CRenderSkinnedInstancing* crowdSkinnedMesh = character->addComponent<CRenderSkinnedInstancing>();
		crowdSkinnedMesh->initFromPrefab(prefab);
		crowdSkinnedMesh->initTextureTransform(animationData, totalFrames, numBones, boneMap);

		// body
		material[1]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinInstaning.xml");
		material[1]->autoDetectLoadTexture();

		// hair
		material[0]->changeShader("BuiltIn/Shader/SpecularGlossiness/Forward/SGSkinInstaning.xml");
		material[0]->autoDetectLoadTexture();

		crowdSkinnedMesh->initMaterial(material);

		// spawn instancing
		for (int i = -5; i < 5; i++)
		{
			for (int j = -5; j < 5; j++)
			{
				CEntity* entity = crowdSkinnedMesh->spawn();

				float time = os::Randomizer::frand() * clip1->Duration;
				crowdSkinnedMesh->setAnimation(entity, 0, clip1, time, 60);

				CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
				transform->Relative.setTranslation(core::vector3df(i * 2.0f, 0.0f, j * 2.0f));
			}
		}

		// free data
		delete[]animationData;
		delete[]transforms;
	}

	// Rendering
	u32 w = app->getWidth();
	u32 h = app->getHeight();

	CContext* context = CContext::getInstance();

	context->initRenderPipeline(w, h);
	context->setActiveZone(zone);
	context->setActiveCamera(camera);
	context->setGUICamera(guiCamera);
	context->setDirectionalLight(directionalLight);
}

void CViewInit::onDestroy()
{
	m_guiObject->remove();
	delete m_font;
}

void CViewInit::onUpdate()
{
	CContext* context = CContext::getInstance();

	switch (m_initState)
	{
	case CViewInit::DownloadBundles:
	{
		io::IFileSystem* fileSystem = getApplication()->getFileSystem();

		std::vector<std::string> listBundles;
		listBundles.push_back("Common.zip");
		listBundles.push_back("SampleModelsResource.zip");
		listBundles.push_back(getApplication()->getTexturePackageName("SampleModels").c_str());

#ifdef __EMSCRIPTEN__
		const char* filename = listBundles[m_downloaded].c_str();

		if (m_getFile == NULL)
		{
			m_getFile = new CGetFileURL(filename, filename);
			m_getFile->download(CGetFileURL::Get);

			char log[512];
			sprintf(log, "Download asset: %s", filename);
			os::Printer::log(log);
		}
		else
		{
			char log[512];
			sprintf(log, "Download asset: %s - %d%%", filename, m_getFile->getPercent());
			m_textInfo->setText(log);

			if (m_getFile->getState() == CGetFileURL::Finish)
			{
				// [bundles].zip
				fileSystem->addFileArchive(filename, false, false);

				if (++m_downloaded >= listBundles.size())
					m_initState = CViewInit::InitScene;
				else
				{
					delete m_getFile;
					m_getFile = NULL;
				}
			}
			else if (m_getFile->getState() == CGetFileURL::Error)
			{
				// retry download
				delete m_getFile;
				m_getFile = NULL;
			}
		}
#else

		for (std::string& bundle : listBundles)
		{
			const char* r = bundle.c_str();
#if defined(WINDOWS_STORE)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#elif defined(MACOS)
			fileSystem->addFileArchive(getBuiltInPath(r), false, false);
#else
			fileSystem->addFileArchive(r, false, false);
#endif
		}

		m_initState = CViewInit::InitScene;
#endif
	}
	break;
	case CViewInit::InitScene:
	{
		initScene();
		m_initState = CViewInit::Finished;
	}
	break;
	case CViewInit::Error:
	{
		// todo nothing with black screen
	}
	break;
	default:
	{
		CScene* scene = context->getScene();
		if (scene != NULL)
			scene->update();

		CViewManager::getInstance()->getLayer(0)->changeView<CViewDemo>();
	}
	break;
	}
}

void CViewInit::onRender()
{
	if (m_initState == CViewInit::Finished)
	{
		CContext* context = CContext::getInstance();
		CScene* scene = CContext::getInstance()->getScene();
		CBaseRP* rp = CContext::getInstance()->getRenderPipeline();
		CCamera* camera = context->getActiveCamera();

		if (m_bakeSHLighting == true)
		{
			m_bakeSHLighting = false;

			CZone* zone = scene->getZone(0);

			// light probe
			CGameObject* lightProbeObj = zone->createEmptyObject();
			CLightProbe* lightProbe = lightProbeObj->addComponent<CLightProbe>();
			lightProbeObj->getTransformEuler()->setPosition(core::vector3df(0.0f, 1.0f, 0.0f));

			CGameObject* bakeCameraObj = scene->getZone(0)->createEmptyObject();
			CCamera* bakeCamera = bakeCameraObj->addComponent<CCamera>();
			scene->updateAddRemoveObject();

			// bake light probe
			Lightmapper::CLightmapper* lm = Lightmapper::CLightmapper::getInstance();
			lm->initBaker(64);

			std::vector<CLightProbe*> probes;
			probes.push_back(lightProbe);

			lm->bakeProbes(probes, bakeCamera, rp, scene->getEntityManager());
		}
	}
	else
	{
		CCamera* guiCamera = CContext::getInstance()->getGUICamera();
		CGraphics2D::getInstance()->render(guiCamera);
	}
}