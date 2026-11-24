import { TestBed } from '@angular/core/testing';

import { MisAjustesService } from './mis-ajustes.service';

describe('MisAjustesService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: MisAjustesService = TestBed.get(MisAjustesService);
    expect(service).toBeTruthy();
  });
});
