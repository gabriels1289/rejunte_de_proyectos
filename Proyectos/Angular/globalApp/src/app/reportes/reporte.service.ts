import { Injectable } from '@angular/core';

import { BehaviorSubject } from 'rxjs';
import { map } from 'rxjs/operators';

import { Reporte } from './reporte';
import { REPORTES } from './mock-reportes';
import { MessageService } from '../_helpers/message.service';

@Injectable({
  providedIn: 'root',
})
export class ReporteService {
  static nextReporteId = 100;
  private reportes$: BehaviorSubject<Reporte[]> = new BehaviorSubject<Reporte[]>(REPORTES);

  constructor(private messageService: MessageService) { }

  getReportes() { return this.reportes$; }

  getReporte(id: number | string) {
    return this.getReportes().pipe(
      // (+) before 'id' turns the string into a number
      map(reportes => reportes.find(reporte => reporte.id === +id))
    );
  }
}
