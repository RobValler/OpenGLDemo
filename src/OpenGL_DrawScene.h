
/*
 ============================================================================
 Name        :
 Author      :
 Version     :
 Copyright   :
 Description :
 ============================================================================
 */



class DrawScene {
public:
	DrawScene(void*);
	virtual ~DrawScene();

	/*declaration of public methods*/
	void* API_RenderingFunc(void * ptr);
	void API_SetShutdownMode();

	static void *ProcessFunction(void *pObj);

protected:



	bool areWeBeingDestroyed;


};

