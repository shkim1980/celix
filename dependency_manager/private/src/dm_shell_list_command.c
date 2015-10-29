/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * dm_shell_list_command.c
 *
 *  \date       Oct 16, 2015
 *  \author    	<a href="mailto:dev@celix.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */
#include <stdlib.h>
#include <string.h>
#include <dm_dependency_manager.h>
#include "dm_info.h"
#include "service_reference.h"
#include "command_impl.h"
#include "array_list.h"
#include "bundle_context.h"
#include "bundle.h"
#include "shell.h"


static const char * const OK_COLOR = "\033[92m";
static const char * const WARNING_COLOR = "\033[93m";
static const char * const NOK_COLOR = "\033[91m";
static const char * const END_COLOR = "\033[m";

char * dmListCommand_getName(command_pt command) {
    return "dm";
}

char * dmListCommand_getUsage(command_pt command) {
    return "dm [overview|notavail]";
}

char * dmListCommand_getShortDescription(command_pt command) {
    return "\t overview: Get an overview of the dependency-managed components with their dependencies.\n\tnotavail: Get an overview of dependency-managed compononentes where required depencies are not available. ";
}

void dmListCommand_execute(command_pt command, char * line, void (*out)(char *), void (*err)(char *)) {
    char outString[256];
    array_list_pt servRefs = NULL;
    int i;
    bundle_context_pt context = (void *)command;
    bundleContext_getServiceReferences(context, DM_INFO_SERVICE_NAME ,NULL, &servRefs);
    char *term = getenv("TERM");
    bool colors = false;
    if (strcmp("xterm-256color", term) == 0) {
        colors = true;
    }

    for(i = 0; i < arrayList_size(servRefs); i++) {
        dm_dependency_manager_info_pt info = NULL;
        dm_info_service_pt infoServ = NULL;
        service_reference_pt servRef = NULL;
        servRef = arrayList_get(servRefs, i);
        bundleContext_getService(context,  servRef, (void**)&infoServ);
        infoServ->getInfo(infoServ->handle, &info);

        int cmpCnt;
        for (cmpCnt = 0; cmpCnt < arrayList_size(info->components); cmpCnt++) {
            dm_component_info_pt compInfo = arrayList_get(info->components, cmpCnt);
            const char *startColors = "";
            const char *endColors = "";
            if (colors) {
                startColors = compInfo->active ? OK_COLOR : NOK_COLOR;
                endColors = END_COLOR;
            }
            sprintf(outString, "Component: Name=%s\n|- ID=%s, %sActive=%s%s, State=%s\n", compInfo->name, compInfo->id, startColors, compInfo->active ?  "true " : "false", endColors, compInfo->state);
            out(outString);

            int interfCnt;
            sprintf(outString, "|- Interfaces (%d):\n", arrayList_size(compInfo->interfaces));
            out(outString);
            for(interfCnt = 0 ;interfCnt < arrayList_size(compInfo->interfaces); interfCnt++) {
                char * interface;
                interface = arrayList_get(compInfo->interfaces, interfCnt);
                sprintf(outString, "   |- Interface: %s\n", interface);
                out(outString);
            }

            int depCnt;
            sprintf(outString, "|- Dependencies (%d):\n", arrayList_size(compInfo->dependency_list));
            out(outString);
            for(depCnt = 0 ;depCnt < arrayList_size(compInfo->dependency_list); depCnt++) {
                dm_service_dependency_info_pt dependency;
                dependency = arrayList_get(compInfo->dependency_list, depCnt);
                const char *startColors = "";
                const char *endColors = "";
                if (colors) {
                    if (dependency->required) {
                        startColors = dependency->available ? OK_COLOR : NOK_COLOR;
                    } else {
                        startColors = dependency->available ? OK_COLOR : WARNING_COLOR;
                    }

                    endColors = END_COLOR;
                }
                sprintf(outString, "   |- Dependency: %sAvailable = %s%s, Required = %s, Filter = %s\n",
                        startColors,
                        dependency->available ? "true " : "false" ,
                        endColors,
                        dependency->required ? "true " : "false",
                        dependency->filter
                );
                out(outString);
            }
            sprintf(outString, "\n");
            out(outString);

            infoServ->destroyInfo(infoServ->handle, info);
        }
    }
}
