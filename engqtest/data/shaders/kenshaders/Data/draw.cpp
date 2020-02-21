void SetMainCamera(int s_pgID3);
void ILS::MeshRenderInstance::render(ILS::RenderContext *context) {
 
    if (!m_Mesh->getSkin())
    {
        m_CanRender = true;
        m_Model = m_Node->getTransform();
    }
 
    ILS::RenderSystem *renderSystem = Platform::getInstance()->getRenderSystem();
    int caps = 0;
 
    RenderContext::PassType passType = context->getPassType();
 
    bool caustics = passType == ILS::RenderContext::PT_CAUSTICS_LEFT
            || context->getPassType() == ILS::RenderContext::PT_CAUSTICS_RIGHT;
 
 
    if (caustics)return;
 
    if (caustics)
    {
        caps = Shader::SC_CAUSTICS;
    }
    else
    {
        if (m_Material)
        {
            caps = m_Material->getCaps();
        }
        else
        {
//            DEBUG("Why no material?!?!?!?\n");
            return;
        }
    }
 
    Matrix4 view;
    Matrix4 projView;
 
    if (context->getStereoCamera())
    {
        if (passType == ILS::RenderContext::PT_STEREO_LEFT || passType
                == ILS::RenderContext::PT_CAUSTICS_LEFT) {
            projView = context->getStereoCamera()->getLeftProjViewMatrix();
            view = context->getStereoCamera()->getLeftViewMatrix();
        } else {
            projView = context->getStereoCamera()->getRightProjViewMatrix();
            view = context->getStereoCamera()->getRightViewMatrix();
        }
    }
    else
    {
        projView = context->getCamera()->getProjViewMatrix();
 
        if (passType == ILS::RenderContext::PT_STEREO_LEFT || passType
                == ILS::RenderContext::PT_CAUSTICS_LEFT) {
            projView = projView * Matrix4::translation(
                    Vector3(0.05f, 0.0f, 0.0f));
            view = context->getCamera()->getViewMatrix()
                    * Matrix4::translation(Vector3(0.05f, 0.0f, 0.0f));
        } else {
            projView = projView * Matrix4::translation(
                    Vector3(-0.05f, 0.0f, 0.0f));
            view = context->getCamera()->getViewMatrix()
                    * Matrix4::translation(Vector3(-0.05f, 0.0f, 0.0f));
        }
    }
 
    Matrix4 model = m_Node->getTransform();
    Matrix4 mvp = projView * model;
 
    Vector3 min;
    Vector3 max;
 
    // TODO: Correctly compute a bounding box for animated objects
    //if (!m_Mesh->getSkin()) {
    Vector3 corners[8];
    Box meshBox(m_Mesh->getMinBounds(), m_Mesh->getMaxBounds(), model);
    meshBox.extractCorners(corners);
 
    //    ... not beautiful, it simply takes the minimum and maximum positions
 
    min = minPerElem(
            minPerElem(minPerElem(corners[0], corners[1]),
                    minPerElem(corners[2], corners[3])),
            minPerElem(minPerElem(corners[4], corners[5]),
                    minPerElem(corners[6], corners[7])));
    max = maxPerElem(
            maxPerElem(maxPerElem(corners[0], corners[1]),
                    maxPerElem(corners[2], corners[3])),
            maxPerElem(maxPerElem(corners[4], corners[5]),
                    maxPerElem(corners[6], corners[7])));
 
    if (!context->getCamera()->aabbInsideFrustum(min, max)) {
        m_Visible = false;
        return;
    }
 
    for (unsigned int i = 0; i < context->getOccPlaneCount(); i++) {
        if (context->getOccPlane(i)->aabbInsideFrustum(max, min)) {
            m_Visible = false;
            return;
        }
    }
 
    m_Visible = true;
 
    Vector3 pos = (min + max) / 2.f;
    pos = (inverse(view) * Vector4(pos, 1.f)).getXYZ();
    m_RenderPriority = -pos.getZ();
 
    Matrix4 normal = inverse(transpose(model));
 
    Matrix4 wvit = inverse(transpose(view * model));
    Matrix4 wv = view * model;
 
    Shader *shader = renderSystem->getDefaultShader(caps);
    renderSystem->setCurrentShader(shader);
 
 
 
    //Debug for display showing data...
    //Using main camera.....
    ILS::GLShader *s = (ILS::GLShader *)shader;
    SetMainCamera((int)s->getProgramName());
 
    if (caps & Shader::SC_FUNSTON) {
        shader->setVector3f("C1", 0.16f, 0.13f, 0.09f);
        shader->setVector3f("C2", -0.02f, -0.02f, 0.f);
        shader->setVector3f("C3", 0.60f, 0.61f, 0.62f);
        shader->setVector3f("C4", -0.14f, -0.14f, -0.13f);
        shader->setVector3f("C5", 0.14f, 0.12f, 0.09f);
        shader->setVector3f("C6", -0.07f, -0.08f, -0.09f);
        shader->setVector3f("C7", -0.46f, -0.44f, -0.35f);
        shader->setVector3f("C8", 0.33f, 0.38f, 0.45f);
        shader->setVector3f("C9", -0.17f, -0.17f, -0.17f);
    }
 
 
    shader->setMatrix4fv("WorldViewProjection", (float*) &(transpose(mvp)));
    shader->setMatrix4fv("WorldInverseTranspose", (float*) &normal);
    shader->setMatrix4fv("WorldViewInverseTranspose", (float*) &wvit);
    shader->setMatrix4fv("View", (float*) &view);
    shader->setMatrix4fv("WorldView", (float*) &wv);
    shader->setMatrix4fv("ProjectionMatrix",
            (float*) &(transpose(
                    context->getProjectionMatrix() * model * Matrix4(
                            Vector4(0.015f, 0.f, 0.f, 0.f),
                            Vector4(0.f, 0.015f, 0.f, 0.f),
                            Vector4(0.f, 0.f, 0.015f, 0.f),
                            Vector4(0.5f, 0.5f, 0.015f, 1.f)))));
 
    //shader->nodeVector3f("LampDirection", 0.0f, -1.0f, 1.0f);
    //shader->setVector3f("LampColor", 1.0f, 1.0f, 1.0f);
 
    shader->setVector4f("fogColor", context->getRedClear(),
            context->getGreenClear(), context->getBlueClear(), 1.f);
    shader->setFloat(
            "cameraDistance",
            1.f / (context->getCamera()->getFar()
                    - context->getCamera()->getNear()));
 
    if ((caps & Shader::SC_TERRAIN) || (caps & Shader::SC_TERRAIN_1C) || (caps
            & Shader::SC_TERRAIN_2C)) {
        if (context->getProjectionTexture())
            renderSystem->bindTexture2D(context->getProjectionTexture(), 4);
        shader->setInteger("caustics", 4);
    }
 
    if (caustics) {
 
        renderSystem->setBlending(true, RenderSystem::BF_DST_COLOR,
                RenderSystem::BF_ONE);
        renderSystem->setDepth(true, RenderSystem::TT_EQUAL);
 
    } else {
 
        renderSystem->setDepth(true, RenderSystem::TT_LESS_EQUAL);
 
        if (m_Material) {
            renderSystem->setCull(m_Material->getCullingEnabled());
            renderSystem->setBlending(m_Material->getBlendEnabled(),
                    m_Material->getSourceBlendFactor(),
                    m_Material->getDestBlendFactor(),
                    m_Material->getBlendEquation());
 
            if (m_Material->getDepthTestEnabled()) {
                renderSystem->setDepth(true, m_Material->getDepthTestType());
            } else {
                renderSystem->setDepth(false, RenderSystem::TT_LESS_EQUAL);
            }
            renderSystem->setDepthWrite(m_Material->getDepthWriteEnabled());
 
        } else {
            renderSystem->setBlending(false);
            renderSystem->setCull(true);
            renderSystem->setDepth(true, RenderSystem::TT_LESS_EQUAL);
            renderSystem->setDepthWrite(true);
        }
    }
 
 
    if (m_Material && !caustics)
    {
        for (unsigned int i = 0; i < m_Material->getTextureCount(); i++)
        {
            renderSystem->bindTexture2D(m_Material->getTexture(i), i);
        }
 
        for (unsigned int i = 0; i < m_Material->getPropertyCount(); i++) {
 
            MaterialProperty *prop = m_Material->getProperty(i);
 
            switch (prop->getPropertyType()) {
            case MaterialProperty::PT_FLOAT:
                shader->setFloat(prop->getPropertyName(),
                        *prop->getFloatValue());
                break;
            case MaterialProperty::PT_FLOAT2:
                shader->setVector2fv(prop->getPropertyName(),
                        prop->getFloatValue());
                break;
            case MaterialProperty::PT_FLOAT3:
 
                shader->setVector3fv(prop->getPropertyName(),
                        prop->getFloatValue());
                break;
            case MaterialProperty::PT_FLOAT4:
                shader->setVector4fv(prop->getPropertyName(),
                        prop->getFloatValue());
                break;
            case MaterialProperty::PT_INT:
                shader->setInteger(prop->getPropertyName(),
                        *prop->getIntValue());
            }
 
        }
 
    }
 
    //updateSkin();
    //if (m_NeedsBufferUpdate)
    //updateBoneMatrices();
    //updateSkin();
    updateVertexBuffer();
 
//Actually Render the Polys Here....
 
    renderSystem->enableAttribSource(ILS::RenderSystem::VAT_POSITION);
    renderSystem->enableAttribSource(ILS::RenderSystem::VAT_UV0);
    renderSystem->enableAttribSource(ILS::RenderSystem::VAT_UV1);
    renderSystem->enableAttribSource(ILS::RenderSystem::VAT_COLOR0);
    renderSystem->enableAttribSource(ILS::RenderSystem::VAT_NORMAL);
 
    renderSystem->setVertexAttribHBSource(m_VertexBuffer, 0, 3,
            RenderSystem::F_FLOAT, sizeof(MeshVertex), false,
            RenderSystem::VAT_POSITION);
 
    renderSystem->setVertexAttribHBSource(m_VertexBuffer,
            offsetof(MeshVertex, m_UV0), 2, RenderSystem::F_FLOAT,
            sizeof(MeshVertex), false, RenderSystem::VAT_UV0);
 
    renderSystem->setVertexAttribHBSource(m_VertexBuffer,
            offsetof(MeshVertex, m_UV1), 2, RenderSystem::F_FLOAT,
            sizeof(MeshVertex), false, RenderSystem::VAT_UV1);
 
    renderSystem->setVertexAttribHBSource(m_VertexBuffer,
            offsetof(MeshVertex, m_Color), 4, RenderSystem::F_UNSIGNED_BYTE,
            sizeof(MeshVertex), true, RenderSystem::VAT_COLOR0);
 
    renderSystem->setVertexAttribHBSource(m_VertexBuffer,
            offsetof(MeshVertex, m_Normal), 3, RenderSystem::F_FLOAT,
            sizeof(MeshVertex), false, RenderSystem::VAT_NORMAL);
 
 
    unsigned int count = m_FaceList.getSize() * 3;
    if (count > m_Mesh->getTriangleList()->m_VertexCount)
    {
        count = m_Mesh->getTriangleList()->m_VertexCount;
    }
 
    //renderSystem->drawArrays(ILS::RenderSystem::PT_TRIANGLE, 0, m_Mesh->getTriangleList()->m_VertexCount);
    //renderSystem->drawArrays(ILS::RenderSystem::PT_TRIANGLE, 0, count);
 
    if (!m_IndexBuffer)
    {
        //DEBUG("Updating index buffer... %s\n", m_Mesh->getName());
        updateIndexBuffer();
    }
    else
    {
        if (m_NeedsIndexUpdate)
        {
            updateIndexBuffer();
        }
 
        //DEBUG("ibDrawElements - %s -> %s\n", m_Mesh->getName(), m_Scene);
        if (m_CanRender) renderSystem->ibDrawElements(ILS::RenderSystem::PT_TRIANGLE,
                m_IndexBuffer, m_FaceList.getSize() * 3,
                RenderSystem::IT_UNSIGNED_SHORT);
    }
 
    renderSystem->disableAttribSource(ILS::RenderSystem::VAT_UV0);
    renderSystem->disableAttribSource(ILS::RenderSystem::VAT_UV1);
    renderSystem->disableAttribSource(ILS::RenderSystem::VAT_POSITION);
    renderSystem->disableAttribSource(ILS::RenderSystem::VAT_NORMAL);
    renderSystem->disableAttribSource(ILS::RenderSystem::VAT_COLOR0);
 
    renderSystem->setDepth(true, RenderSystem::TT_LESS_EQUAL);
    renderSystem->setBlending(false);
    renderSystem->setDepthWrite(true);
    //debugRender(context);
}
