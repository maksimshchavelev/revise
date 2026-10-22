// Copyright 2025 Maksim Shchavelev <maksimshchavelev@gmail.com>

#pragma once

namespace core {

enum class Permission {
    POST_NOTIFICATIONS,
    INTERNET,
    SCHEDULE_EXACT_ALARM,
    READ_EXTERNAL_STORAGE,
    WRITE_EXTERNAL_STORAGE
};

/**
 * @brief Permission request interface
 *
 * This class is needed to request permission on a specific platform. Behavior depends on implementation. You must
 * implement the `IPermissionService::request` and `IPermissionService::check` methods yourself, depending on the
 * platform.
 *
 * @note An object of this class must live long enough.
 */
class IPermissionService {
  public:
    virtual ~IPermissionService() = default;

    /**
     * @brief Checks whether permission has been granted
     * @param Permit name
     * @return `true` if permission is granted
     * @note Implement this method yourself
     */
    virtual bool check(Permission permission) const noexcept = 0;

    /**
     * @brief Request permission
     * @param permission Permit name
     * @note Implement this method yourself
     */
    virtual void request(Permission permission) const noexcept = 0;
};

} // namespace core
