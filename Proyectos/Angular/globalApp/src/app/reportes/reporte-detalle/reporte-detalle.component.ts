import { Component, OnInit, HostBinding } from '@angular/core';
import { Router, ActivatedRoute } from '@angular/router';
import { Observable } from 'rxjs';

import { Reporte } from '../reporte';
import { DialogService } from '../../_helpers/dialog.service';

@Component({
  selector: 'app-reporte-detalle',
  templateUrl: './reporte-detalle.component.html',
  styleUrls: ['./reporte-detalle.component.css']
})
export class ReporteDetalleComponent implements OnInit {
  nombreEditado: string;
  reporte: Reporte;

  constructor(
    private route: ActivatedRoute,
    private router: Router,
    public dialogService: DialogService
    ) { }

  ngOnInit() {
    this.route.data
    .subscribe((data: { reporte: Reporte }) => {
      this.nombreEditado = data.reporte.nombre;
      this.reporte = data.reporte;
    });
  }

  cancel() {
    this.gotoReportes();
  }

  save() {
    this.reporte.nombre = this.nombreEditado;
    this.gotoReportes();
  }

  gotoReportes() {
    // esto era let pero el lint sugiere const
    const reporteId = this.reporte ? this.reporte.id : null;
    // Pass along the reporte id if available
    // so taht the ReporteLista component can select that reporte.
    // this.router.navigate(['/reportes', {id: reporteId}]);
    this.router.navigate(['../', { id: reporteId }], { relativeTo: this.route });
  }

  canDeactivate(): Observable<boolean> | boolean {
    // Allows synchronous navigation (`true`) if no reporte or the reporte is unchanged
    if (!this.reporte || this.reporte.nombre === this.nombreEditado) {
      return true;
    }
    // Otherwise ask the user with the dialog service and return its
    // observable wich resolves to true or false when the user decides
    return this.dialogService.confirm('Descartar cambios?');
  }

}
