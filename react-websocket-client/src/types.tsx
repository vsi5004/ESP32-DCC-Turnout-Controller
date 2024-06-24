export interface TurnoutSetting {
  id: number;
  address: number;
  closedEndpoint: number;
  openEndpoint: number;
  reversed: boolean;
  testInProgress: boolean;
  throwSpeed: number;
}
