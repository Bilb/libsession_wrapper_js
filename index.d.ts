declare module "session_util_wrapper" {
  export abstract class BaseConfigWrapper {
    public needsDump(): boolean;
    public needsPush(): boolean;
    public push(): { data: Uint8Array; seqno: number };
    public dump(): Uint8Array;
    public confirmPushed(seqno: number): void;
    public merge(toMerge: Array<Uint8Array>): number;
    public storageNamespace(): number;
    public encryptionDomain(): string;
  }
  export class UserConfigWrapper extends BaseConfigWrapper {
    constructor(secretKey: Uint8Array, dump: Uint8Array | null);
    public getName(): string;
    public setName(name: string);
    public getProfilePic(): {
      url: string | null;
      key: Uint8Array | null;
    };
    public setProfilePic(url: string | null, key: Uint8Array | null);
  }
}
