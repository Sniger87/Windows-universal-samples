//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
//-----------------------------------------------------------------------------
// <auto-generated> 
//   This code was generated by a tool. 
// 
//   Changes to this file may cause incorrect behavior and will be lost if  
//   the code is regenerated.
//
//   Tool: AllJoynCodeGenerator.exe
//
//   This tool is located in the Windows 10 SDK and the Windows 10 AllJoyn 
//   Visual Studio Extension in the Visual Studio Gallery.  
//
//   The generated code should be packaged in a Windows 10 C++/CX Runtime  
//   Component which can be consumed in any UWP-supported language using 
//   APIs that are available in Windows.Devices.AllJoyn.
//
//   Using AllJoynCodeGenerator - Invoke the following command with a valid 
//   Introspection XML file and a writable output directory:
//     AllJoynCodeGenerator -i <INPUT XML FILE> -o <OUTPUT DIRECTORY>
// </auto-generated>
//-----------------------------------------------------------------------------
#include "pch.h"

using namespace concurrency;
using namespace Microsoft::WRL;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Devices::AllJoyn;
using namespace org::alljoyn::Onboarding;

std::map<alljoyn_interfacedescription, WeakReference*> OnboardingConsumer::SourceInterfaces;

OnboardingConsumer::OnboardingConsumer(AllJoynBusAttachment^ busAttachment)
    : m_busAttachment(busAttachment),
    m_proxyBusObject(nullptr),
    m_busObject(nullptr),
    m_sessionListener(nullptr),
    m_sessionId(0)
{
    m_weak = new WeakReference(this);
    m_signals = ref new OnboardingSignals();
    m_nativeBusAttachment = AllJoynHelpers::GetInternalBusAttachment(m_busAttachment);
}

OnboardingConsumer::~OnboardingConsumer()
{
    AllJoynBusObjectManager::ReleaseBusObject(m_nativeBusAttachment, AllJoynHelpers::PlatformToMultibyteString(ServiceObjectPath).data());
    if (SessionListener != nullptr)
    {
        alljoyn_busattachment_setsessionlistener(m_nativeBusAttachment, m_sessionId, nullptr);
        alljoyn_sessionlistener_destroy(SessionListener);
    }
    if (nullptr != ProxyBusObject)
    {
        alljoyn_proxybusobject_destroy(ProxyBusObject);
    }
    delete m_weak;
}

void OnboardingConsumer::OnSessionLost(_In_ alljoyn_sessionid sessionId, _In_ alljoyn_sessionlostreason reason)
{
    if (sessionId == m_sessionId)
    {
        AllJoynSessionLostEventArgs^ args = ref new AllJoynSessionLostEventArgs(static_cast<AllJoynSessionLostReason>(reason));
        SessionLost(this, args);
    }
}

void OnboardingConsumer::OnSessionMemberAdded(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberAddedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberAdded(this, args);
    }
}

void OnboardingConsumer::OnSessionMemberRemoved(_In_ alljoyn_sessionid sessionId, _In_ PCSTR uniqueName)
{
    if (sessionId == m_sessionId)
    {
        auto args = ref new AllJoynSessionMemberRemovedEventArgs(AllJoynHelpers::MultibyteToPlatformString(uniqueName));
        SessionMemberRemoved(this, args);
    }
}

QStatus OnboardingConsumer::AddSignalHandler(_In_ alljoyn_busattachment busAttachment, _In_ alljoyn_interfacedescription interfaceDescription, _In_ PCSTR methodName, _In_ alljoyn_messagereceiver_signalhandler_ptr handler)
{
    alljoyn_interfacedescription_member member;
    if (!alljoyn_interfacedescription_getmember(interfaceDescription, methodName, &member))
    {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    return alljoyn_busattachment_registersignalhandler(busAttachment, handler, member, NULL);
}

IAsyncOperation<OnboardingJoinSessionResult^>^ OnboardingConsumer::JoinSessionAsync(
    _In_ AllJoynServiceInfo^ serviceInfo, _Inout_ OnboardingWatcher^ watcher)
{
    return create_async([serviceInfo, watcher]() -> OnboardingJoinSessionResult^
    {
        auto result = ref new OnboardingJoinSessionResult();
        result->Consumer = ref new OnboardingConsumer(watcher->BusAttachment);
        result->Status = result->Consumer->JoinSession(serviceInfo);
        return result;
    });
}

IAsyncOperation<OnboardingConfigureWiFiResult^>^ OnboardingConsumer::ConfigureWiFiAsync(_In_ Platform::String^ interfaceMemberSSID, _In_ Platform::String^ interfaceMemberPassphrase, _In_ int16 interfaceMemberAuthType)
{
    return create_async([this, interfaceMemberSSID, interfaceMemberPassphrase, interfaceMemberAuthType]() -> OnboardingConfigureWiFiResult^
    {
        auto result = ref new OnboardingConfigureWiFiResult();
        
        alljoyn_message message = alljoyn_message_create(m_nativeBusAttachment);
        size_t argCount = 3;
        alljoyn_msgarg inputs = alljoyn_msgarg_array_create(argCount);

        QStatus status = ER_OK;
        status = static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(inputs, 0), "s", interfaceMemberSSID));
	
        if (ER_OK == status)
        {
            status = static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(inputs, 1), "s", interfaceMemberPassphrase));
        }
	
        if (ER_OK == status)
        {
            status = static_cast<QStatus>(TypeConversionHelpers::SetAllJoynMessageArg(alljoyn_msgarg_array_element(inputs, 2), "n", interfaceMemberAuthType));
        }
	
        if (ER_OK == status)
        {
            status = alljoyn_proxybusobject_methodcall(
                ProxyBusObject,
                "org.alljoyn.Onboarding",
                "ConfigureWiFi",
                inputs,
                argCount,
                message,
                c_MessageTimeoutInMilliseconds,
                0);
        }
        result->Status = static_cast<int>(status);
        if (ER_OK == status) 
        {
            result->Status = AllJoynStatus::Ok;
            int16 argument0;
            status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "n", &argument0));
            result->Status2 = argument0;

            if (ER_OK != status)
            {
                result->Status = static_cast<int>(status);
            }
        }
        else if (ER_BUS_REPLY_IS_ERROR_MESSAGE == status)
        {
            alljoyn_msgarg errorArg = alljoyn_message_getarg(message, 1);
            if (nullptr != errorArg)
            {
                uint16 errorStatus;
                status = alljoyn_msgarg_get_uint16(errorArg, &errorStatus);
                if (ER_OK == status)
                {
                    status = static_cast<QStatus>(errorStatus);
                }
            }
            result->Status = static_cast<int>(status);
        }
        
        alljoyn_message_destroy(message);
        alljoyn_msgarg_destroy(inputs);

        return result;
    });
}
IAsyncOperation<OnboardingConnectResult^>^ OnboardingConsumer::ConnectAsync()
{
    return create_async([this]() -> OnboardingConnectResult^
    {
        auto result = ref new OnboardingConnectResult();
        
        alljoyn_message message = alljoyn_message_create(m_nativeBusAttachment);
        size_t argCount = 0;
        alljoyn_msgarg inputs = alljoyn_msgarg_array_create(argCount);

        QStatus status = ER_OK;
        if (ER_OK == status)
        {
            status = alljoyn_proxybusobject_methodcall(
                ProxyBusObject,
                "org.alljoyn.Onboarding",
                "Connect",
                inputs,
                argCount,
                message,
                c_MessageTimeoutInMilliseconds,
                0);
        }
        result->Status = static_cast<int>(status);
        
        alljoyn_message_destroy(message);
        alljoyn_msgarg_destroy(inputs);

        return result;
    });
}
IAsyncOperation<OnboardingOffboardResult^>^ OnboardingConsumer::OffboardAsync()
{
    return create_async([this]() -> OnboardingOffboardResult^
    {
        auto result = ref new OnboardingOffboardResult();
        
        alljoyn_message message = alljoyn_message_create(m_nativeBusAttachment);
        size_t argCount = 0;
        alljoyn_msgarg inputs = alljoyn_msgarg_array_create(argCount);

        QStatus status = ER_OK;
        if (ER_OK == status)
        {
            status = alljoyn_proxybusobject_methodcall(
                ProxyBusObject,
                "org.alljoyn.Onboarding",
                "Offboard",
                inputs,
                argCount,
                message,
                c_MessageTimeoutInMilliseconds,
                0);
        }
        result->Status = static_cast<int>(status);
        
        alljoyn_message_destroy(message);
        alljoyn_msgarg_destroy(inputs);

        return result;
    });
}
IAsyncOperation<OnboardingGetScanInfoResult^>^ OnboardingConsumer::GetScanInfoAsync()
{
    return create_async([this]() -> OnboardingGetScanInfoResult^
    {
        auto result = ref new OnboardingGetScanInfoResult();
        
        alljoyn_message message = alljoyn_message_create(m_nativeBusAttachment);
        size_t argCount = 0;
        alljoyn_msgarg inputs = alljoyn_msgarg_array_create(argCount);

        QStatus status = ER_OK;
        if (ER_OK == status)
        {
            status = alljoyn_proxybusobject_methodcall(
                ProxyBusObject,
                "org.alljoyn.Onboarding",
                "GetScanInfo",
                inputs,
                argCount,
                message,
                c_MessageTimeoutInMilliseconds,
                0);
        }
        result->Status = static_cast<int>(status);
        if (ER_OK == status) 
        {
            result->Status = AllJoynStatus::Ok;
            uint16 argument0;
            status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "q", &argument0));
            result->Age = argument0;

            if (ER_OK != status)
            {
                result->Status = static_cast<int>(status);
            }
            Windows::Foundation::Collections::IVector<OnboardingScanListItem^>^ argument1;
            status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 1), "a(sn)", &argument1));
            result->ScanList = argument1;

            if (ER_OK != status)
            {
                result->Status = static_cast<int>(status);
            }
        }
        else if (ER_BUS_REPLY_IS_ERROR_MESSAGE == status)
        {
            alljoyn_msgarg errorArg = alljoyn_message_getarg(message, 1);
            if (nullptr != errorArg)
            {
                uint16 errorStatus;
                status = alljoyn_msgarg_get_uint16(errorArg, &errorStatus);
                if (ER_OK == status)
                {
                    status = static_cast<QStatus>(errorStatus);
                }
            }
            result->Status = static_cast<int>(status);
        }
        
        alljoyn_message_destroy(message);
        alljoyn_msgarg_destroy(inputs);

        return result;
    });
}

IAsyncOperation<OnboardingGetVersionResult^>^ OnboardingConsumer::GetVersionAsync()
{
    return create_async([this]() -> OnboardingGetVersionResult^
    {
        PropertyGetContext<uint16> getContext;
        
        alljoyn_proxybusobject_getpropertyasync(
            ProxyBusObject,
            "org.alljoyn.Onboarding",
            "Version",
            [](QStatus status, alljoyn_proxybusobject obj, const alljoyn_msgarg value, void* context)
            {
                UNREFERENCED_PARAMETER(obj);
                auto propertyContext = static_cast<PropertyGetContext<uint16>*>(context);

                if (ER_OK == status)
                {
                    uint16 argument;
                    status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(value, "q", &argument));

                    propertyContext->SetValue(argument);
                }
                propertyContext->SetStatus(status);
                propertyContext->SetEvent();
            },
            c_MessageTimeoutInMilliseconds,
            &getContext);

        getContext.Wait();

        auto result = ref new OnboardingGetVersionResult();
        result->Status = getContext.GetStatus();
        result->Version = getContext.GetValue();
        return result;
    });
}

IAsyncOperation<OnboardingGetStateResult^>^ OnboardingConsumer::GetStateAsync()
{
    return create_async([this]() -> OnboardingGetStateResult^
    {
        PropertyGetContext<int16> getContext;
        
        alljoyn_proxybusobject_getpropertyasync(
            ProxyBusObject,
            "org.alljoyn.Onboarding",
            "State",
            [](QStatus status, alljoyn_proxybusobject obj, const alljoyn_msgarg value, void* context)
            {
                UNREFERENCED_PARAMETER(obj);
                auto propertyContext = static_cast<PropertyGetContext<int16>*>(context);

                if (ER_OK == status)
                {
                    int16 argument;
                    status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(value, "n", &argument));

                    propertyContext->SetValue(argument);
                }
                propertyContext->SetStatus(status);
                propertyContext->SetEvent();
            },
            c_MessageTimeoutInMilliseconds,
            &getContext);

        getContext.Wait();

        auto result = ref new OnboardingGetStateResult();
        result->Status = getContext.GetStatus();
        result->State = getContext.GetValue();
        return result;
    });
}

IAsyncOperation<OnboardingGetLastErrorResult^>^ OnboardingConsumer::GetLastErrorAsync()
{
    return create_async([this]() -> OnboardingGetLastErrorResult^
    {
        PropertyGetContext<OnboardingLastError^> getContext;
        
        alljoyn_proxybusobject_getpropertyasync(
            ProxyBusObject,
            "org.alljoyn.Onboarding",
            "LastError",
            [](QStatus status, alljoyn_proxybusobject obj, const alljoyn_msgarg value, void* context)
            {
                UNREFERENCED_PARAMETER(obj);
                auto propertyContext = static_cast<PropertyGetContext<OnboardingLastError^>*>(context);

                if (ER_OK == status)
                {
                    OnboardingLastError^ argument;
                    status = static_cast<QStatus>(TypeConversionHelpers::GetAllJoynMessageArg(value, "(ns)", &argument));

                    propertyContext->SetValue(argument);
                }
                propertyContext->SetStatus(status);
                propertyContext->SetEvent();
            },
            c_MessageTimeoutInMilliseconds,
            &getContext);

        getContext.Wait();

        auto result = ref new OnboardingGetLastErrorResult();
        result->Status = getContext.GetStatus();
        result->LastError = getContext.GetValue();
        return result;
    });
}

void OnboardingConsumer::OnPropertyChanged(_In_ alljoyn_proxybusobject obj, _In_ PCSTR interfaceName, _In_ const alljoyn_msgarg changed, _In_ const alljoyn_msgarg invalidated)
{
    UNREFERENCED_PARAMETER(obj);
    UNREFERENCED_PARAMETER(interfaceName);
    UNREFERENCED_PARAMETER(changed);
    UNREFERENCED_PARAMETER(invalidated);
}

void OnboardingConsumer::CallConnectionResultSignalHandler(_In_ const alljoyn_interfacedescription_member* member, _In_ alljoyn_message message)
{
    auto source = SourceInterfaces.find(member->iface);
    if (source == SourceInterfaces.end())
    {
        return;
    }

    auto consumer = source->second->Resolve<OnboardingConsumer>();
    if (consumer->Signals != nullptr)
    {
        auto callInfo = ref new AllJoynMessageInfo(AllJoynHelpers::MultibyteToPlatformString(alljoyn_message_getsender(message)));
        auto eventArgs = ref new OnboardingConnectionResultReceivedEventArgs();
        eventArgs->MessageInfo = callInfo;

        Onboarding^ argument0;
        (void)TypeConversionHelpers::GetAllJoynMessageArg(alljoyn_message_getarg(message, 0), "(ns)", &argument0);

        eventArgs->Arg = argument0;

        consumer->Signals->CallConnectionResultReceived(consumer->Signals, eventArgs);
    }
}

int32 OnboardingConsumer::JoinSession(_In_ AllJoynServiceInfo^ serviceInfo)
{
    alljoyn_sessionlistener_callbacks callbacks =
    {
        AllJoynHelpers::SessionLostHandler<OnboardingConsumer>,
        AllJoynHelpers::SessionMemberAddedHandler<OnboardingConsumer>,
        AllJoynHelpers::SessionMemberRemovedHandler<OnboardingConsumer>
    };

    alljoyn_busattachment_enableconcurrentcallbacks(AllJoynHelpers::GetInternalBusAttachment(m_busAttachment));

    SessionListener = alljoyn_sessionlistener_create(&callbacks, m_weak);
    alljoyn_sessionopts sessionOpts = alljoyn_sessionopts_create(ALLJOYN_TRAFFIC_TYPE_MESSAGES, true, ALLJOYN_PROXIMITY_ANY, ALLJOYN_TRANSPORT_ANY);

    std::vector<char> sessionNameUtf8 = AllJoynHelpers::PlatformToMultibyteString(serviceInfo->UniqueName);
    RETURN_IF_QSTATUS_ERROR(alljoyn_busattachment_joinsession(
        m_nativeBusAttachment,
        &sessionNameUtf8[0],
        serviceInfo->SessionPort,
        SessionListener,
        &m_sessionId,
        sessionOpts));
    alljoyn_sessionopts_destroy(sessionOpts);

    ServiceObjectPath = serviceInfo->ObjectPath;
    std::vector<char> objectPath = AllJoynHelpers::PlatformToMultibyteString(ServiceObjectPath);

    if (objectPath.empty())
    {
        return AllJoynStatus::Fail;
    }

    ProxyBusObject = alljoyn_proxybusobject_create(m_nativeBusAttachment, &sessionNameUtf8[0], &objectPath[0], m_sessionId);
    if (nullptr == ProxyBusObject)
    {
        return AllJoynStatus::Fail;
    }


    alljoyn_interfacedescription description = alljoyn_busattachment_getinterface(m_nativeBusAttachment, "org.alljoyn.Onboarding");
    if (nullptr == description)
    {
        return AllJoynStatus::Fail;
    }

    RETURN_IF_QSTATUS_ERROR(AllJoynBusObjectManager::GetBusObject(m_nativeBusAttachment, AllJoynHelpers::PlatformToMultibyteString(ServiceObjectPath).data(), &m_busObject));
   
    if (!AllJoynBusObjectManager::BusObjectIsRegistered(m_nativeBusAttachment, m_busObject))
    {
        RETURN_IF_QSTATUS_ERROR(alljoyn_busobject_addinterface(BusObject, description));
    }

    QStatus result = AddSignalHandler(
        m_nativeBusAttachment,
        description,
        "ConnectionResult",
        [](const alljoyn_interfacedescription_member* member, PCSTR srcPath, alljoyn_message message) { UNREFERENCED_PARAMETER(srcPath); CallConnectionResultSignalHandler(member, message); });
    if (ER_OK != result)
    {
        return static_cast<int32>(result);
    }

    SourceInterfaces[description] = m_weak;

    unsigned int noneMechanismIndex = 0;
    bool authenticationMechanismsContainsNone = m_busAttachment->AuthenticationMechanisms->IndexOf(AllJoynAuthenticationMechanism::None, &noneMechanismIndex);
    QCC_BOOL interfaceIsSecure = alljoyn_interfacedescription_issecure(description);

    // If the current set of AuthenticationMechanisms supports authentication, 
    // determine whether to secure the connection.
    if (AllJoynHelpers::CanSecure(m_busAttachment->AuthenticationMechanisms))
    {
        // Secure the connection if the org.alljoyn.Bus.Secure XML annotation
        // is specified, or if None is not present in AuthenticationMechanisms.
        if (!authenticationMechanismsContainsNone || interfaceIsSecure)
        {
            RETURN_IF_QSTATUS_ERROR(alljoyn_proxybusobject_secureconnection(ProxyBusObject, QCC_FALSE));
            RETURN_IF_QSTATUS_ERROR(AllJoynBusObjectManager::TryRegisterBusObject(m_nativeBusAttachment, BusObject, true));
        }
        else
        {
            RETURN_IF_QSTATUS_ERROR(AllJoynBusObjectManager::TryRegisterBusObject(m_nativeBusAttachment, BusObject, false));
        }
    }
    else
    {
        // If the current set of AuthenticationMechanisms does not support authentication
        // but the interface requires security, report an error.
        if (interfaceIsSecure)
        {
            return static_cast<int32>(ER_BUS_NO_AUTHENTICATION_MECHANISM);
        }
        else
        {
            RETURN_IF_QSTATUS_ERROR(AllJoynBusObjectManager::TryRegisterBusObject(m_nativeBusAttachment, BusObject, false));
        }
    }

    RETURN_IF_QSTATUS_ERROR(alljoyn_proxybusobject_addinterface(ProxyBusObject, description));
    
    m_signals->Initialize(BusObject, m_sessionId);

    return AllJoynStatus::Ok;
}
