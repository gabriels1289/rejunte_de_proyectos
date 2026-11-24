import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { Observable } from 'rxjs';
import { switchMap } from 'rxjs/operators';

import { Reporte } from '../reporte';
import { ReporteService } from '../reporte.service';

@Component({
  selector: 'app-reportes-lista',
  templateUrl: './reportes-lista.component.html',
  styleUrls: ['./reportes-lista.component.css']
})
export class ReportesListaComponent implements OnInit {

  reportes$: Observable<Reporte[]>;
  selectedId: number;

  constructor(
    private service: ReporteService,
    private route: ActivatedRoute
    ) { }

  ngOnInit() {
    this.reportes$ = this.route.paramMap.pipe(
      switchMap(params => {
        // (+) before `params.get()` turns the string into a number
        this.selectedId = +params.get('id');
        return this.service.getReportes();
      })
    );
  }

}
