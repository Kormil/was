#include "secretsmanager.h"

#include <QObject>
#include <QtQml>
#include <QVariant>
#include <sailfishapp.h>

SecretsManager::SecretsManager() {
}

unsigned int SecretsManager::addRequest(SecretRequestPtr request) {
    auto serial = serial_.fetch_add(1, std::memory_order_relaxed);

    requests_.insert({serial, std::move(request)});
    return serial;
}

void SecretsManager::isCollectionExistsInSecretsManager(std::function<void(void)> handler) {
    auto request_ptr = std::make_shared<Sailfish::Secrets::CollectionNamesRequest>();
    request_ptr->setManager(&secrets_manager_);
    request_ptr->setStoragePluginName(Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName);

    auto request_raw_ptr = request_ptr.get();
    auto serial = addRequest(std::move(request_ptr));

    QObject::connect(request_raw_ptr, &Sailfish::Secrets::CollectionNamesRequest::collectionNamesChanged, [serial, handler, this]() {
        SecretRequestPtr request;
        if (auto search = requests_.find(serial); search != requests_.end()) {
            if (search->second->status() != Sailfish::Secrets::Request::Finished) {
                return;
            }

            request = search->second;
            requests_.erase(serial);
        } else {
            qWarning() << "Cannot find CollectionNamesRequest with serial: " << serial;
            return;
        }

        if (request->result().code() == Sailfish::Secrets::Result::Failed) {
            qWarning() << "Failed to get collections names: "
                         << request->result().errorMessage();
            return;
        }

        auto cnr = static_cast<Sailfish::Secrets::CollectionNamesRequest *>(request.get());
        if (cnr->collectionNames().contains(COLLECTION_NAME)) {
            collection_exists_ = true;
            return handler();
        } else {
            createSecretsCollection(handler);
        }
    });

    request_raw_ptr->startRequest();
}

void SecretsManager::createSecretsCollection(std::function<void(void)> handler) {
    auto request_ptr = std::make_shared<Sailfish::Secrets::CreateCollectionRequest>();
    request_ptr->setManager(&secrets_manager_);
    request_ptr->setCollectionName(COLLECTION_NAME);
    request_ptr->setAccessControlMode(Sailfish::Secrets::SecretManager::OwnerOnlyMode);
    request_ptr->setCollectionLockType(Sailfish::Secrets::CreateCollectionRequest::DeviceLock);
    request_ptr->setDeviceLockUnlockSemantic(Sailfish::Secrets::SecretManager::DeviceLockKeepUnlocked);
    request_ptr->setStoragePluginName(Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName);
    request_ptr->setEncryptionPluginName(Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName);

    auto request_raw_ptr = request_ptr.get();
    auto serial = addRequest(std::move(request_ptr));

    QObject::connect(request_raw_ptr, &Sailfish::Secrets::CreateCollectionRequest::statusChanged, [serial, handler, this]() {
        SecretRequestPtr request;
        if (auto search = requests_.find(serial); search != requests_.end()) {
            if (search->second->status() != Sailfish::Secrets::Request::Finished) {
                return;
            }

            request = search->second;
            requests_.erase(serial);
        } else {
            qWarning() << "Cannot find CreateCollectionRequest with serial: " << serial;
            return;
        }

        if (request->status() != Sailfish::Secrets::Request::Finished) {
            return;
        }

        if (request->result().code() == Sailfish::Secrets::Result::Failed) {
            qWarning() << "Failed to create collection: "
                        << request->result().errorMessage();
            return;
        }

        collection_exists_ = true;
        return handler();
    });

    request_raw_ptr->startRequest();
}

void SecretsManager::saveToSecretsManager(const QString& name, const QString& value) {
    if (!collection_exists_) {
        return;
    }

    // Define the secret.
     Sailfish::Secrets::Secret secret(
             Sailfish::Secrets::Secret::Identifier(
                     name,
                     COLLECTION_NAME,
                     Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName));

     secret.setData(value.toUtf8());
     secret.setType(Sailfish::Secrets::Secret::TypeBlob);

     // Request that the secret be securely stored.
     auto request_ptr = std::make_shared<Sailfish::Secrets::StoreSecretRequest>();
     request_ptr->setManager(&secrets_manager_);
     request_ptr->setSecretStorageType(Sailfish::Secrets::StoreSecretRequest::CollectionSecret);
     request_ptr->setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);
     request_ptr->setSecret(secret);

     auto request_raw_ptr = request_ptr.get();
     auto serial = addRequest(std::move(request_ptr));

     QObject::connect(request_raw_ptr, &Sailfish::Secrets::StoreSecretRequest::statusChanged, [serial, name, this]() {
         SecretRequestPtr request;
         if (auto search = requests_.find(serial); search != requests_.end()) {
             if (search->second->status() != Sailfish::Secrets::Request::Finished) {
                 return;
             }

             request = search->second;
             requests_.erase(serial);
         } else {
             qWarning() << "Cannot find StoreSecretRequest with serial: " << serial;
             return;
         }

         if (request->result().code() == Sailfish::Secrets::Result::Failed) {
             qWarning() << "Failed to save "
                        << name
                        << ": "
                        << request->result().errorMessage();
             return;
         }
     });

     request_raw_ptr->startRequest();
}

void SecretsManager::readFromSecretsManager(const QString& name, std::function<void(QString)> handler) {
    if (!collection_exists_) {
        return handler("");
    }

    Sailfish::Secrets::Secret::Identifier identifier(
                name,
                COLLECTION_NAME,
                Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName);

    auto request_ptr = std::make_shared<Sailfish::Secrets::StoredSecretRequest>();
    request_ptr->setManager(&secrets_manager_);
    request_ptr->setIdentifier(identifier);
    request_ptr->setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);

    auto request_raw_ptr = request_ptr.get();
    auto serial = addRequest(request_ptr);

    QObject::connect(request_raw_ptr, &Sailfish::Secrets::StoredSecretRequest::statusChanged, [serial, name, handler, this]() {
        SecretRequestPtr request;
        if (auto search = requests_.find(serial); search != requests_.end()) {
            if (search->second->status() != Sailfish::Secrets::Request::Finished) {
                return handler("");
            }

            request = search->second;
            requests_.erase(serial);
        } else {
            qWarning() << "Cannot find StoredSecretRequest with serial: " << serial;
            return handler("");
        }

        QString data("");
        auto ssr = static_cast<Sailfish::Secrets::StoredSecretRequest *>(request.get());

        if (!ssr) {
            qWarning() << "Badcast StoredSecretRequest: " << serial;
            return handler("");
        }

        if (request->result().code() == Sailfish::Secrets::Result::Failed) {
            qWarning() << "Failed to read "
                       << name
                       << ": "
                       << ssr->result().errorMessage();
        } else {
            data = ssr->secret().data();
        }

        handler(data);
    });

    request_raw_ptr->startRequest();
}

void SecretsManager::deleteFromSecretsManager(const QString& name, std::function<void(bool)> handler) {
    if (!collection_exists_) {
        return handler("");
    }

    Sailfish::Secrets::Secret::Identifier identifier(
                name,
                COLLECTION_NAME,
                Sailfish::Secrets::SecretManager::DefaultEncryptedStoragePluginName);

    auto request_ptr = std::make_shared<Sailfish::Secrets::DeleteSecretRequest>();
    request_ptr->setManager(&secrets_manager_);
    request_ptr->setIdentifier(identifier);
    request_ptr->setUserInteractionMode(Sailfish::Secrets::SecretManager::SystemInteraction);

    auto request_raw_ptr = request_ptr.get();
    auto serial = addRequest(request_ptr);

    QObject::connect(request_raw_ptr, &Sailfish::Secrets::DeleteSecretRequest::statusChanged, [serial, name, handler, this]() {
        SecretRequestPtr request;
        if (auto search = requests_.find(serial); search != requests_.end()) {
            if (search->second->status() != Sailfish::Secrets::Request::Finished) {
                return handler(false);
            }

            request = search->second;
            requests_.erase(serial);
        } else {
            qWarning() << "Cannot find DeleteSecretRequest with serial: " << serial;
            return handler(false);
        }

        auto dsr = static_cast<Sailfish::Secrets::DeleteSecretRequest *>(request.get());

        if (!dsr) {
            qWarning() << "Badcast DeleteSecretRequest: " << serial;
            return handler(false);
        }

        if (request->result().code() == Sailfish::Secrets::Result::Failed) {
            qWarning() << "Failed to delete "
                       << name
                       << ": "
                       << dsr->result().errorMessage();
        }

        handler(true);
    });

    request_raw_ptr->startRequest();
}
