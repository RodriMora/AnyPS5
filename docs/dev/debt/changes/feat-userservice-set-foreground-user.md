# libSceUserService

## add [sceUserServiceSetForegroundUser]

- [sceUserServiceSetForegroundUser](../../../core/libs/prx/libSceUserService/Export.cpp) (libSceUserService) - the 13.60 managed wrapper recovers `(int userId)`; AnyPS5 stores the id for `sceUserServiceGetForegroundUser` to copy out, starting from the initial user id, and rejects an id that maps to no user slot with `USER_SERVICE_ERROR_UNKNOWN_USER` (0x80960105), the check `sceUserServiceGetUserNumber` and `sceUserServiceGetUserColor` make. Whether the console rejects unknown ids, which error it reports there and whether the setter refuses a user who is not logged in are unmeasured.
