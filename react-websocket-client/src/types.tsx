export type TurnoutSettingValue = number | boolean | string;

export interface TurnoutSetting {
  id: number;
  address: number;
  closedEndpoint: number;
  openEndpoint: number;
  reversed: boolean;
  startClosed: boolean;
  moveInProgress: boolean;
  throwSpeed: number;
  poweredFrog: boolean;
  reverseFrogPolarity: boolean;
}

export const TURNOUT_MIN_THROW_SPEED = 1;
export const TURNOUT_MAX_THROW_SPEED = 20;
export const TURNOUT_MIN_ENDPOINT = 0;
export const TURNOUT_MAX_ENDPOINT = 180;
