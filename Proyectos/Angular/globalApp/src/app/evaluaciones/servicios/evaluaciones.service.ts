import { Injectable } from '@angular/core';
import { Observable } from 'rxjs';
import { catchError } from 'rxjs/operators';

// esto es para intentar acceder a los empleados mediante la base de datos
import { HttpClient, HttpParams, HttpHeaders, HttpResponse } from '@angular/common/http';
import { HttpErrorHandlerService, handleError } from '../../_helpers/http-error-handler.service';

// importo la direccion de la api
import { environment } from '../../../environments/environment';  // TODO: cambiar a prod

@Injectable({
  providedIn: 'root'
})
export class EvaluacionesService {

  private handleError: handleError;

  constructor(
  private http: HttpClient,
  httpErrorhandler: HttpErrorHandlerService
  ) {
    this.handleError = httpErrorhandler.createHandleError('EvaluacionesService');
  }

  getListadoEvaluaciones(idusuario: number, idsector: number) {
    const  params = new  HttpParams().set('idusuario', idusuario.toString())
                                      .set('idsector', idsector.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/listado`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getListadoEvaluaciones', []))
      );
  }

  getResumenEvaluacion(idevaluacion: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/listado/resumen`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getResumenEvaluacion', []))
      );
  }

  getParaEvaluar(idevaluacion: number, idsector: number, idsupervisor: number, idmodelo: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsector', idsector.toString())
                                      .set('idsupervisor', idsupervisor.toString())
                                      .set('idmodelo', idmodelo.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/evaluar`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getParaEvaluar', []))
      );
  }

  putArchivar(data: any): Observable<any> {
    return this.http.put<any>(`${environment.apiUrl}/index.php/evaluaciones/listado`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('putArchivar', []))
      );
  }

  putAbrirCerrar(data: any): Observable<any> {
    return this.http.put<any>(`${environment.apiUrl}/index.php/evaluaciones/listado/abrir-cerrar`, data, {observe: 'response'})
      .pipe(
        catchError(this.handleError('putAbrirCerrar', []))
      );
  }

  deleteEvaluacion(idevaluacion: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString());
    return this.http.delete(`${environment.apiUrl}/index.php/evaluaciones/listado`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('deleteEvaluacion', []))
      );
  }

  postEvaluacionEmpleado(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/evaluaciones/evaluar`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postEvaluacionEmpleado', []))
      );
  }

  postDevolucion(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/evaluaciones/comparacion`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postEvaluacionEmpleado', []))
      );
  }

  getRankingEvaluacion(idevaluacion: number, idsupervisor: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsupervisor', idsupervisor.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/evaluar/ranking`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getRankingEvaluacion', []))
      );
  }

  getRankingGlobal(idevaluacion: number, idsupervisor: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsupervisor', idsupervisor.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/evaluar/ranking-global`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getRankingGlobal', []))
      );
  }

  getListadoBigBrother(idevaluacion: number, idsupervisor: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsupervisor', idsupervisor.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/evaluar/bigBrother`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getListadoBigBrother', []))
      );
  }

  getEvaluados(idevaluacion: number, idsupervisor: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsupervisor', idsupervisor.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/comparacion`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getEvaluados', []))
      );
  }

  getComparacion(idevaluacion: number, idevaluado: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idevaluado', idevaluado.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/comparacion/comparaciones`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getComparacion', []))
      );
  }

  getDevolucion(idevaluacion: number, idevaluado: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idevaluado', idevaluado.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/comparacion/devolucion`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDevolucion', []))
      );
  }


  getDetalleEvaluacion(idevaluacion: number, idsupervisor: number, idevaluado: number) {
    const  params = new  HttpParams().set('idevaluacion', idevaluacion.toString())
                                      .set('idsupervisor', idsupervisor.toString())
                                      .set('idevaluado', idevaluado.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/evaluar/edicion`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDetalleEvaluacion', []))
      );
  }

  getDatosParaNuevaEvaluacion(idsupervisor: number, listaPermitidos: string) {
    const  params = new  HttpParams().set('idsupervisor', idsupervisor.toString())
                                      .set('listaPermitidos', listaPermitidos);
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/crear-evaluacion`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDatosParaNuevaEvaluacion', []))
      );
  }

  getPlantilla(idmodelo: number) {
    const  params = new  HttpParams().set('idmodelo', idmodelo.toString());
    return this.http.get(`${environment.apiUrl}/index.php/evaluaciones/crear-evaluacion/plantilla`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getPlantilla', []))
      );
  }

  postNuevoComp(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/evaluaciones/crear-evaluacion/comp`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postNuevoComp', []))
      );
  }

  postNuevaEvaluacion(formData: FormData): Observable<any> {
    return this.http.post<any>(`${environment.apiUrl}/index.php/evaluaciones/crear-evaluacion`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postNuevaEvaluacion', []))
      );
  }


}
