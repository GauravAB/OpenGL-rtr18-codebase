

	extern static vector<vector<float>> vertices;
	extern static vector<vector<float>> normals;
	extern static vector<vector<float>> textures;
	extern static vector<vector<float>> face_indices;

	int parseObj(const char*);

	void getModelVertexData(float *vertices , float* textures , float* normals , short elements);
	void processModelData(void);
	