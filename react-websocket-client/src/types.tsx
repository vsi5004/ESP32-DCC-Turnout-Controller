export interface TurnoutSetting {
  id: number;
  address: number;
  closedEndpoint: number;
  openEndpoint: number;
  reversed: boolean;
  startClosed: boolean;
  testInProgress: boolean;
  throwSpeed: number;
  poweredFrog: boolean;
  reverseFrogPolarity: boolean;
}
