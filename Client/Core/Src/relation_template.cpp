//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-1-11
//      File_base        : relation_template
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 关系模版
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "relation_template.h"

RelationTemplate::RelationTemplate()
{
	Clear();
}

RelationTemplate::~RelationTemplate()
{	
}

void RelationTemplate::Clear()
{
	m_Loaded = false;
	memset(m_Name, 0 ,sizeof(m_Name));
	memset(m_Desc, 0, sizeof(m_Desc));
	m_Id = 0;
	m_LayerCount = 0;
	memset(m_Layers, 0, sizeof(m_Layers));
}

bool RelationTemplate::Load(const char* templateFileName)
{
	if (templateFileName == NULL)
	{
		_ASSERT(false);
		return false;
	}

	Clear();

	KIniFile file;
	if (TRUE == file.Load(templateFileName))
	{	
		file.GetInteger("Relation", "Id", 0, &m_Id);
		file.GetInteger("Relation", "Layer", 0, &m_LayerCount);
		file.GetString("Relation", "Name", "", m_Name, sizeof(m_Name));
		file.GetString("Relation", "Desc", "", m_Desc, sizeof(m_Desc));
		file.GetInteger("Relation", "SaveFlag", 0, &m_IsSave);

		char layerSectionName[64] = { 0 };
		char fieldName[64] = { 0 };
		char itemIdStrBuff[64] = { 0 };
		for (int layerLoopCount = 0; layerLoopCount < m_LayerCount; layerLoopCount++)
		{
			RelationLayer& relationLayer = m_Layers[layerLoopCount];
			sprintf(layerSectionName, "Layer%d", layerLoopCount + 1);

			file.GetInteger(layerSectionName, "MinChildCount", 0, &(relationLayer.MinChildCount));
			file.GetInteger(layerSectionName, "MaxChildCount", 0, &(relationLayer.MaxChildCount));
			file.GetString(layerSectionName, "Name", "", relationLayer.Name, sizeof(relationLayer.Name));
#ifndef _SERVER
			file.GetString(layerSectionName, "Desc", "", relationLayer.Desc, sizeof(relationLayer.Desc));
			file.GetString(layerSectionName, "Controller", "", relationLayer.Controller, sizeof(relationLayer.Controller));
#endif
					
			int operationCount = 0;
			file.GetInteger(layerSectionName, "OperationCount", 0, &(relationLayer.OperationCount));
			for(int operationLoopCount = 0; operationLoopCount < relationLayer.OperationCount; operationLoopCount++)
			{
				RelationOperation& operation = relationLayer.Operations[operationCount];

				sprintf(fieldName, "Operation%d_Id", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.Id));
				sprintf(fieldName, "Operation%d_IsDefault", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.IsDefault));
				sprintf(fieldName, "Operation%d_RequireMoney", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.RequireMoney));
				sprintf(fieldName, "Operation%d_RequireLevel", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.RequireLevel));
				sprintf(fieldName, "Operation%d_ReqTargetLvl", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.ReqTargetLevel));
				sprintf(fieldName, "Operation%d_RequireItem", operationLoopCount + 1);
				file.GetString(layerSectionName, fieldName, "", itemIdStrBuff, sizeof(itemIdStrBuff));
				sprintf(fieldName, "Operation%d_ToAll", operationLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(operation.IsToAll));
				sscanf(itemIdStrBuff, "%d,%d,%d,%d", &operation.RequireItem.IDArray[0], &operation.RequireItem.IDArray[1], &operation.RequireItem.IDArray[2], &operation.RequireItem.IDArray[3]);

#ifndef _SERVER
				for (int controllerLoopCount = 0; controllerLoopCount < MAX_CONTROLLER_COUNT; controllerLoopCount++)
				{
					sprintf(fieldName, "Operation%d_Controller%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetString(layerSectionName, fieldName, "", operation.Controllers[controllerLoopCount].Controller, MAX_NAME_LENGTH);
					sprintf(fieldName, "Operation%d_Event%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetString(layerSectionName, fieldName, "", operation.Controllers[controllerLoopCount].Event, MAX_NAME_LENGTH);
					sprintf(fieldName, "Operation%d_Name%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetString(layerSectionName, fieldName, "", operation.Controllers[controllerLoopCount].Name, MAX_NAME_LENGTH);
					sprintf(fieldName, "Operation%d_Desc%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetString(layerSectionName, fieldName, "", operation.Controllers[controllerLoopCount].Desc, MAX_DES_LENGTH);				
					sprintf(fieldName, "Operation%d_ReturnController%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetString(layerSectionName, fieldName, "", operation.Controllers[controllerLoopCount].ReturnController, MAX_NAME_LENGTH);
					sprintf(fieldName, "Operation%d_NeedConfirm%d", operationLoopCount + 1, controllerLoopCount + 1);
					file.GetInteger(layerSectionName, fieldName, 0, &(operation.Controllers[controllerLoopCount].NeedConfirm));
				}
#endif

#ifdef _SERVER
				if (operation.Id >= 0)
				{
					operationCount++;
				}
#else
				operationCount++;
#endif
			}

			relationLayer.OperationCount = operationCount;

			file.GetInteger(layerSectionName, "AttributeCount", 0, &(relationLayer.AttributeCount));
			for(int attributeLoopCount = 0; attributeLoopCount < relationLayer.AttributeCount; attributeLoopCount++)
			{
				sprintf(fieldName, "Attribute%d_Id", attributeLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(relationLayer.Attributes[attributeLoopCount].Id));
				sprintf(fieldName, "Attribute%d_SaveFlag", attributeLoopCount + 1);
				file.GetInteger(layerSectionName, fieldName, 0, &(relationLayer.Attributes[attributeLoopCount].SaveFlag));
			}

			file.GetInteger(layerSectionName, "JoinBuff", 0, &(relationLayer.JoinBuff));
			file.GetInteger(layerSectionName, "LeaveBuff", 0, &(relationLayer.LeaveBuff));
			file.GetInteger(layerSectionName, "IsCreateChatChannel", 0, &(relationLayer.IsCreateChatChannel));
			file.GetInteger(layerSectionName, "BuffMustNotHaveOnJoin", 0, &(relationLayer.BuffMustNotHaveOnJoin));
			file.GetInteger(layerSectionName, "CityTaxRateBuff", 0, &(relationLayer.CityTaxRateBuff));

			for (int i = 0; i < 10; ++i)
			{
				sprintf(fieldName, "ChangeOwnerBuff_%d", i);
				file.GetInteger(layerSectionName, fieldName, 0, &(relationLayer.ChangeOwnerBuff[i]));
			}
		}

		m_Loaded = true;
		return true;
	}	

	return false;
}

const PRelationLayer RelationTemplate::GetLayer(int layer)
{
	if (!m_Loaded)
	{
		_ASSERT(false);
		return NULL;
	}

	if (layer <= 0 || layer > m_LayerCount)
	{
		_ASSERT(false);
		return NULL;
	}

	return &m_Layers[layer - 1];
}

RelationTemplateManager::RelationTemplateManager()
{
	m_Loaded = false;
}

RelationTemplateManager::~RelationTemplateManager()
{
}

RelationTemplateManager& RelationTemplateManager::Singleton()
{
	static RelationTemplateManager manager;
	return manager;
}

bool RelationTemplateManager::Load(const char* templateCfgFileName)
{
	if (templateCfgFileName == NULL)
	{
		_ASSERT(false);
		return false;
	}

	m_Loaded = false;
	m_Templates.clear();

	KIniFile file;	
	if (TRUE == file.Load(templateCfgFileName))
	{
		int templateCount = 0;
		file.GetInteger("Templates", "TemplateCount", 0, &templateCount);

		for (int templateLoopCount = 0; templateLoopCount < templateCount; templateLoopCount++)
		{
			char templateName[64] = { 0 };
			char templateFileName[128] = { 0 };
			sprintf(templateName, "Template%d", templateLoopCount + 1);
			if (TRUE == file.GetString("Templates", templateName, "", templateFileName, sizeof(templateFileName)))			
			{
				static RelationTemplate relationTemplate;
				if (relationTemplate.Load(templateFileName))
				{
					m_Templates.insert(m_Templates.end(), relationTemplate);
				}
				else
				{
					_ASSERT(false);//关系模版载入失败
				}
			}
		}
		
		m_Loaded = true;
		return true;
	}

	return false;
}

RelationTemplate* RelationTemplateManager::GetTemplate(int templateId)
{
	if (!m_Loaded)
	{
		_ASSERT(false);
		return NULL;
	}

	RelationTemplateArray::iterator iterCurr = m_Templates.begin();
	RelationTemplateArray::iterator iterEnd = m_Templates.end();

	while (iterCurr != iterEnd)
	{
		RelationTemplate& relationTemplate = *iterCurr;
		if (relationTemplate.GetId() == templateId)
		{
			return &relationTemplate;
		}
		
		++iterCurr;
	}

	return NULL;
}
