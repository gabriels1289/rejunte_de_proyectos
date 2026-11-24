import { TestBed } from '@angular/core/testing';

import { ReporteDetalleResolverService } from './reporte-detalle-resolver.service';

describe('ReporteDetalleResolverService', () => {
  beforeEach(() => TestBed.configureTestingModule({}));

  it('should be created', () => {
    const service: ReporteDetalleResolverService = TestBed.get(ReporteDetalleResolverService);
    expect(service).toBeTruthy();
  });
});
