import { Injectable } from '@angular/core';

import { Observable } from 'rxjs';
import { catchError } from 'rxjs/operators';

import { Empleado, EmpleadoListado } from '../comunes/empleado';

// esto es para intentar acceder a los empleados mediante la base de datos
import { HttpClient, HttpParams, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpErrorHandlerService, handleError } from '../../_helpers/http-error-handler.service';

// importo la direccion de la api
import { environment } from '../../../environments/environment';  // TODO: cambiar a prod
import { HistorialSeguimiento } from 'src/app/seguimientos/comunes/seguimiento';

@Injectable({
  providedIn: 'root'
})
export class EmpleadosService {

  private handleError: handleError;

  constructor(
  private http: HttpClient,
  httpErrorhandler: HttpErrorHandlerService
  ) {
    this.handleError = httpErrorhandler.createHandleError('EmpleadoService');
  }

  getListadoDeEmpleados(idacceso: number, listahijos: string): Observable<any[] | HttpResponse<EmpleadoListado[]>> {
    const  params = new  HttpParams().set('idacceso', idacceso.toString())
                                      .set('listahijos', listahijos);
    return this.http.get<EmpleadoListado[]>(`${environment.apiUrl}/index.php/empleados/listado`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getListadoDeEmpleados', []))
      );
  }

  getHistorialDeSeguimientos(idempleado: number): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idempleado', idempleado.toString());
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/seguimientos`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getHistorialDeSeguimientos', []))
      );
  }

  deleteSeguimiento(idseguimiento: number): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idseguimiento', idseguimiento.toString());
    return this.http.delete<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/seguimientos`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('deleteSeguimiento', []))
      );
  }

  postNuevoSeguimiento(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/empleados/seguimientos`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postNuevoSeguimiento', []))
      );
  }

  getDetalleEmpleado(idempleado: number): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idempleado', idempleado.toString());
    // tslint:disable-next-line: max-line-length
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/datos-personales`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDetalleEmpleado', []))
      );
  }

  resetearPassword(data: any): Observable<any> {
    return this.http.put<any>(`${environment.apiUrl}/index.php/empleados/datos-personales`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('resetPassword', []))
      );
  }

  getDatosSelectsFormulario(idacceso: number): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idacceso', idacceso.toString());
    // tslint:disable-next-line: max-line-length
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/datos-personales/formulario`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDatosSelectsFormulario', []))
      );
  }

  postEdicionEmpleado(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/empleados/datos-personales`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postEdicionEmpleado', []))
      );
  }

  getSeguimientosEnSala(idacceso: number, listahijos: string): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idacceso', idacceso.toString())
                                      .set('listahijos', listahijos);
    // tslint:disable-next-line: max-line-length
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/seguimientos/sala`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getSeguimientosEnSala', []))
      );
  }

  getEvaluacionesDeEmpleado(idevaluado: number, origen: string): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idevaluado', idevaluado.toString())
                                      .set('origen', origen);
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/evaluaciones`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getEvaluacionesDeEmpleado', []))
      );
  }

  getDetalleEvaluacionEmpleado(idevaluado: number, idevaluacion: number): Observable<any[] | HttpResponse<HistorialSeguimiento[]>> {
    const  params = new  HttpParams().set('idevaluado', idevaluado.toString())
                                      .set('idevaluacion', idevaluacion.toString());
    // tslint:disable-next-line: max-line-length
    return this.http.get<HistorialSeguimiento[]>(`${environment.apiUrl}/index.php/empleados/evaluaciones/detalle`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDetalleEvaluacionEmpleado', []))
      );
  }

  getReporteEvaluacionExcel(idevaluado: number, idevaluacion: number, tituloReporte: string, nombreExcel: string) {
    const  params = new  HttpParams().set('idevaluado', idevaluado.toString())
                                      .set('idevaluacion', idevaluacion.toString())
                                      .set('tituloReporte', tituloReporte.toString())
                                      .set('nombreExcel', nombreExcel.toString());
    return this.http.get(`${environment.apiUrl}/index.php/empleados/evaluaciones/reporte`, {responseType: 'blob' as 'json', params})
      .pipe(
        catchError(this.handleError('getReporteEvaluacionExcel', []))
      );
  }

  getResumenEmpleado(idempleado: number) {
    const  params = new  HttpParams().set('idempleado', idempleado.toString());
    return this.http.get(`${environment.apiUrl}/index.php/empleados/datos-personales/resumen`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getResumenEmpleado', []))
      );
  }

  // TODO: redefinir o quitar los HttpResponse<>

}
