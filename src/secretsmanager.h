#ifndef SECRETSMANAGER_H
#define SECRETSMANAGER_H

#include <memory>
#include <atomic>
#include <unordered_map>
#include <functional>

#include <Sailfish/Secrets/secret.h>
#include <Sailfish/Secrets/storesecretrequest.h>
#include <Sailfish/Secrets/storedsecretrequest.h>
#include <Sailfish/Secrets/collectionnamesrequest.h>
#include <Sailfish/Secrets/secretmanager.h>
#include <Sailfish/Secrets/createcollectionrequest.h>
#include <Sailfish/Secrets/deletesecretrequest.h>

class SecretsManager
{
public:
    using SecretRequestPtr = std::shared_ptr<Sailfish::Secrets::Request>;

    explicit SecretsManager();
    
    void isCollectionExistsInSecretsManager(std::function<void(void)> handler);
    void createSecretsCollection(std::function<void(void)> handler);
    void saveToSecretsManager(const QString& name, const QString& value);
    void readFromSecretsManager(const QString& name, std::function<void (QString)> handler);
    void deleteFromSecretsManager(const QString& name, std::function<void(bool)> handler);
    
private:
    Sailfish::Secrets::SecretManager secrets_manager_;

    unsigned int addRequest(SecretRequestPtr request);

    bool collection_exists_ = false;
    std::atomic_uint serial_ = {0};
    std::unordered_map<unsigned int, SecretRequestPtr> requests_;

    const QString COLLECTION_NAME = "WasAppSecrets";
};

#endif // SETTINGS_H
